#!/usr/bin/env bash
#
# project/ 下 Matter 照明工程的编译与清理脚本（Linux）。
#
# 用法:
#   ./build.sh <项目目录名>              # 编译
#   ./build.sh clean <项目目录名>        # 清除编译产物
#   ./build.sh clean all                 # 清除全部项目
#   ./build.sh rebuild <项目目录名>      # 跳过 SLC generate，仅重编译
#   ./build.sh list                      # 列出 project/ 下可编译项目
#   ./build.sh                           # 无参数：列出 project/ 项目
#   source ./build.sh                    # 无参数：列出项目并在当前 shell 安装 Tab 补全
#   ./build.sh --bash-completion         # 输出 Tab 补全定义（供 eval 使用）
#
# 环境变量:
#   BOARD   开发板名称，默认 brd2703a
#   JOBS    并行编译任务数（传给 slc/sl_build.py -j）

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_INVOKER="$(basename "${BASH_SOURCE[0]}")"
PROJECT_DIR="${REPO_ROOT}/project"
SL_BUILD="${REPO_ROOT}/slc/sl_build.py"
DEFAULT_BOARD="${BOARD:-brd2703a}"

declare -A PROJECT_LABEL=(
	[dimmable_light]="调光灯 (Dimmable Light)"
	[colortemperature_light]="色温灯 (Color Temperature Light)"
	[extended_color_light]="五路 RGBCW 灯 (Extended Color Light)"
	[extended_color_light_strip]="SPI 幻彩灯带 (Extended Color Light Strip)"
)

PROJECT_NAMES=()
PROJECT_SLCW=()

# 已知项目显示顺序（仅排序；未列出的目录只要含 .slcw 也会被发现）
PROJECT_ORDER=(
	dimmable_light
	colortemperature_light
	extended_color_light
	extended_color_light_strip
)

# project/ 下不参与编译的目录名
PROJECT_SKIP_DIRS=(scripts)

is_skipped_project_dir() {
	local name="$1"
	local skip
	for skip in "${PROJECT_SKIP_DIRS[@]}"; do
		if [[ "$name" == "$skip" ]]; then
			return 0
		fi
	done
	return 1
}

# 在 project/<name>/ 下查找顶层 solution（*.slcw）
find_project_slcw() {
	local pdir="$1"
	local slcw
	slcw="$(find "$pdir" -maxdepth 1 -name '*.slcw' -print -quit 2>/dev/null || true)"
	if [[ -n "$slcw" && -f "$slcw" ]]; then
		echo "$slcw"
		return 0
	fi
	return 1
}

# 扫描 project/ 子目录，索引所有含 .slcw 的可编译工程
discover_projects() {
	PROJECT_NAMES=()
	PROJECT_SLCW=()
	local name d slcw extra=()
	local -A slcw_map=()

	if [[ ! -d "$PROJECT_DIR" ]]; then
		return 0
	fi

	for d in "${PROJECT_DIR}"/*/; do
		[[ -d "$d" ]] || continue
		name="$(basename "$d")"
		if is_skipped_project_dir "$name"; then
			continue
		fi
		slcw="$(find_project_slcw "$d" || true)"
		if [[ -n "$slcw" ]]; then
			slcw_map["$name"]="$slcw"
		fi
	done

	for name in "${PROJECT_ORDER[@]}"; do
		if [[ -n "${slcw_map[$name]:-}" ]]; then
			PROJECT_NAMES+=("$name")
			PROJECT_SLCW+=("${slcw_map[$name]}")
			unset 'slcw_map[$name]'
		fi
	done

	if [[ ${#slcw_map[@]} -gt 0 ]]; then
		mapfile -t extra < <(printf '%s\n' "${!slcw_map[@]}" | sort)
		for name in "${extra[@]}"; do
			PROJECT_NAMES+=("$name")
			PROJECT_SLCW+=("${slcw_map[$name]}")
		done
	fi
}

is_known_project() {
	local want="$1"
	local name
	for name in "${PROJECT_NAMES[@]}"; do
		if [[ "$name" == "$want" ]]; then
			return 0
		fi
	done
	return 1
}

project_slcw_path() {
	local want="$1"
	local i
	for i in "${!PROJECT_NAMES[@]}"; do
		if [[ "${PROJECT_NAMES[$i]}" == "$want" ]]; then
			echo "${PROJECT_SLCW[$i]}"
			return 0
		fi
	done
	return 1
}

validate_project_arg() {
	local arg="$1"
	if [[ "$arg" == "all" ]]; then
		return 0
	fi
	if is_known_project "$arg"; then
		return 0
	fi
	echo "未找到项目: $arg" >&2
	echo "可用项目: ${PROJECT_NAMES[*]}" >&2
	return 1
}

print_project_list() {
	local name slcw i
	echo "可编译项目（开发板: ${DEFAULT_BOARD}）:"
	for i in "${!PROJECT_NAMES[@]}"; do
		name="${PROJECT_NAMES[$i]}"
		slcw="${PROJECT_SLCW[$i]}"
		printf "  %-28s %s\n" "$name" "${PROJECT_LABEL[$name]:-}"
		printf "      %s\n" "${slcw#${REPO_ROOT}/}"
	done
	echo ""
	echo "编译: ./${BUILD_INVOKER} <项目名>"
	echo "清除: ./${BUILD_INVOKER} clean <项目名>"
}

# 无参数：列出项目；source 时在本 shell 安装 Tab 补全
build_sh_list_and_complete() {
	local sourced=0
	if [[ "${BASH_SOURCE[0]}" != "${0}" ]]; then
		sourced=1
	fi
	print_project_list
	if [[ $sourced -eq 1 ]]; then
		if [[ -t 0 ]]; then
			install_bash_completion || true
			echo "已在本终端启用 ./${BUILD_INVOKER} Tab 补全"
		fi
		return 0
	fi
	if [[ -t 0 ]]; then
		echo ""
		echo "本会话启用 Tab 补全: source ./${BUILD_INVOKER}"
		echo "或: eval \"\$(./${BUILD_INVOKER} --bash-completion)\""
	fi
	return 0
}

clean_product_dir() {
	local pdir="$1"
	local name
	name="$(basename "$pdir")"
	if [[ ! -d "$pdir" ]]; then
		echo "目录不存在: $pdir" >&2
		return 1
	fi
	echo "清理编译产物: ${name}"
	local path removed=0
	while IFS= read -r path; do
		[[ -n "$path" ]] || continue
		rm -rf "$path"
		echo "  已删除: ${path#${REPO_ROOT}/}"
		removed=$((removed + 1))
	done < <(
		find "$pdir" \( \
			-type d -name artifact \
			-o -type d -path '*/cmake_gcc/build' \
			-o -type d -path '*/cmake_iar/build' \
			-o -type d -path '*/cmake_llvm/build' \
			-o -type d -path '*_cmake/build' \
		\) 2>/dev/null | sort -r
	)
	if [[ $removed -eq 0 ]]; then
		echo "  （无编译产物目录）"
	fi
}

clean_projects() {
	local target="$1"
	local name

	validate_project_arg "$target" || return 1

	if [[ "$target" == "all" ]]; then
		for name in "${PROJECT_NAMES[@]}"; do
			clean_product_dir "${PROJECT_DIR}/${name}"
		done
		if [[ -d "${REPO_ROOT}/out" ]]; then
			rm -rf "${REPO_ROOT}/out"
			echo "已删除: out/"
		fi
		return 0
	fi

	clean_product_dir "${PROJECT_DIR}/${target}"
}

run_build() {
	local slcw="$1"
	local skip_gen="$2"
	local board="$3"
	local jobs="${4:-}"

	if [[ ! -f "$SL_BUILD" ]]; then
		echo "缺少编译脚本: ${SL_BUILD}" >&2
		return 1
	fi

	local -a cmd=(python3 "$SL_BUILD" -p "$slcw" -b "$board")
	if [[ "$skip_gen" == "true" ]]; then
		cmd+=(-s)
	fi
	if [[ -n "$jobs" ]]; then
		cmd+=(-j "$jobs")
	fi

	echo "执行: (cd ${REPO_ROOT}) ${cmd[*]}"
	(cd "$REPO_ROOT" && "${cmd[@]}")
}

build_project() {
	local target="$1"
	local skip_gen="$2"
	local board="$3"
	local jobs="${4:-}"
	local name slcw

	validate_project_arg "$target" || return 1

	if [[ "$target" == "all" ]]; then
		for name in "${PROJECT_NAMES[@]}"; do
			echo "======== 编译 ${name} ========"
			slcw="$(project_slcw_path "$name")"
			run_build "$slcw" "$skip_gen" "$board" "$jobs" || return 1
		done
		return 0
	fi

	slcw="$(project_slcw_path "$target")"
	run_build "$slcw" "$skip_gen" "$board" "$jobs"
}

usage() {
	cat <<EOF
用法: $(basename "$0") <项目目录名> [选项]
      $(basename "$0") clean <项目目录名|all> [选项]
      $(basename "$0") rebuild <项目目录名> [选项]

<项目目录名> 为 project/ 下的文件夹名，例如:
  dimmable_light
  colortemperature_light
  extended_color_light
  extended_color_light_strip

选项:
  -b, --board <板型>   开发板，默认 ${DEFAULT_BOARD}
  -j, --jobs <N>       并行任务数
  -s, --skip-gen       编译时跳过 SLC generate
  -h, --help           显示帮助

示例:
  $(basename "$0") colortemperature_light
  $(basename "$0") clean colortemperature_light
  $(basename "$0") clean all
  $(basename "$0") rebuild extended_color_light -j 8
EOF
}

install_bash_completion() {
	if [[ -z "${BASH_VERSION:-}" ]]; then
		return 0
	fi
	# shellcheck disable=SC1090
	eval "$(emit_bash_completion)"
}

# source 时仅列出项目并安装 Tab 补全（禁止 exit，避免关闭当前终端）
build_sh_source_init() {
	discover_projects
	if [[ ${#PROJECT_NAMES[@]} -eq 0 ]]; then
		echo "在 ${PROJECT_DIR} 下未找到可编译工程（各子目录需含顶层 *.slcw）" >&2
		echo "可先执行: python3 project/scripts/setup_light_projects.py" >&2
		return 1
	fi
	build_sh_list_and_complete
	return 0
}

emit_bash_completion() {
	# shellcheck disable=SC2016
	local projects="${PROJECT_NAMES[*]}"
	cat <<COMPLETION
_build_sh_complete() {
	local cur prev
	_cur="\${COMP_WORDS[COMP_CWORD]}"
	_prev="\${COMP_WORDS[COMP_CWORD-1]}"

	if [[ "\${COMP_CWORD}" -eq 1 ]]; then
		COMPREPLY=(\$(compgen -W "clean rebuild list help ${projects}" -- "\$_cur"))
		return
	fi

	if [[ "\${COMP_CWORD}" -eq 2 && "\${COMP_WORDS[1]}" =~ ^(clean|rebuild)$ ]]; then
		COMPREPLY=(\$(compgen -W "${projects} all" -- "\$_cur"))
		return
	fi

	if [[ "\$_prev" == "-b" || "\$_prev" == "--board" ]]; then
		COMPREPLY=(\$(compgen -W "brd2703a brd4187c" -- "\$_cur"))
		return
	fi

	COMPREPLY=(\$(compgen -W "-b --board -j --jobs -s --skip-gen" -- "\$_cur"))
}
complete -o bashdefault -o default -F _build_sh_complete build.sh
complete -o bashdefault -o default -F _build_sh_complete ./build.sh
COMPLETION
}

parse_options() {
	local -n _board=$1
	local -n _jobs=$2
	local -n _skip_gen=$3
	shift 3

	while [[ $# -gt 0 ]]; do
		case "$1" in
		-b | --board)
			_board="$2"
			shift 2
			;;
		-j | --jobs)
			_jobs="$2"
			shift 2
			;;
		-s | --skip-gen)
			_skip_gen=true
			shift
			;;
		-h | --help)
			usage
			exit 0
			;;
		*)
			echo "未知参数: $1" >&2
			usage >&2
			exit 1
			;;
		esac
	done
}

main() {
	if [[ "${1:-}" == "--bash-completion" ]]; then
		discover_projects
		emit_bash_completion
		return 0
	fi

	discover_projects
	if [[ ${#PROJECT_NAMES[@]} -eq 0 ]]; then
		echo "在 ${PROJECT_DIR} 下未找到可编译工程（各子目录需含顶层 *.slcw）" >&2
		echo "可先执行: python3 project/scripts/setup_light_projects.py" >&2
		exit 1
	fi

	local cmd="${1:-}"
	if [[ -z "$cmd" ]]; then
		build_sh_list_and_complete
		exit 0
	fi

	shift || true

	case "$cmd" in
	-h | --help | help)
		usage
		exit 0
		;;
	list | ls)
		print_project_list
		exit 0
		;;
	clean)
		local target="${1:-}"
		shift || true
		if [[ -z "$target" ]]; then
			echo "请指定项目目录名，例如: $(basename "$0") clean colortemperature_light" >&2
			exit 1
		fi
		parse_options board jobs skip_gen "$@"
		clean_projects "$target"
		;;
	rebuild)
		local target="${1:-}"
		shift || true
		if [[ -z "$target" ]]; then
			echo "请指定项目目录名，例如: $(basename "$0") rebuild colortemperature_light" >&2
			exit 1
		fi
		local board="$DEFAULT_BOARD" jobs="" skip_gen=false
		parse_options board jobs skip_gen "$@"
		build_project "$target" true "$board" "$jobs"
		;;
	build)
		local target="${1:-}"
		shift || true
		if [[ -z "$target" ]]; then
			echo "请指定项目目录名，例如: $(basename "$0") colortemperature_light" >&2
			exit 1
		fi
		local board="$DEFAULT_BOARD" jobs="" skip_gen=false
		parse_options board jobs skip_gen "$@"
		if [[ "$skip_gen" == true ]]; then
			build_project "$target" true "$board" "$jobs"
		else
			build_project "$target" false "$board" "$jobs"
		fi
		;;
	*)
		if is_known_project "$cmd"; then
			local board="$DEFAULT_BOARD" jobs="" skip_gen=false
			parse_options board jobs skip_gen "$@"
			if [[ "$skip_gen" == true ]]; then
				build_project "$cmd" true "$board" "$jobs"
			else
				build_project "$cmd" false "$board" "$jobs"
			fi
		else
			echo "未知命令或项目: $cmd" >&2
			echo "可用项目: ${PROJECT_NAMES[*]}" >&2
			usage >&2
			exit 1
		fi
		;;
	esac
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
	set -euo pipefail
	main "$@"
else
	build_sh_source_init
	return $?
fi
