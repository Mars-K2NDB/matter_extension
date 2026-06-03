#!/usr/bin/env bash
# 在 templates/overlays/ 下创建指向各产品 *_app/ 的符号链接。
# setup_light_projects.py 从 overlays 复制时，实际写入的是产品目录中的源文件内容。

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
OVERLAYS="${SCRIPT_DIR}/templates/overlays"

declare -A PRODUCT_APP=(
	[dimmable_light]=dimmable_light_app
	[colortemperature_light]=colortemperature_light_app
	[extended_color_light]=extended_color_light_app
	[extended_color_light_strip]=extended_color_light_strip_app
)

declare -A PRODUCT_FILES=(
	[dimmable_light]="include/light_output.h src/CustomerAppTask.cpp"
	[colortemperature_light]="include/light_output.h"
	[extended_color_light]="include/light_output.h src/CustomerAppTask.cpp"
	[extended_color_light_strip]="include/light_output.h src/CustomerAppTask.cpp"
)

link_one() {
	local product="$1"
	local rel="$2"
	local app_dir="${PRODUCT_APP[$product]}"
	local src="${PROJECT_ROOT}/${product}/${app_dir}/${rel}"
	local link_dir="${OVERLAYS}/${product}/$(dirname "${rel}")"
	local link_path="${link_dir}/$(basename "${rel}")"

	if [[ ! -f "${src}" ]]; then
		echo "跳过（源文件不存在）: ${src}" >&2
		return 1
	fi

	mkdir -p "${link_dir}"
	local rel_target
	rel_target="$(realpath --relative-to="${link_dir}" "${src}")"
	ln -sfn "${rel_target}" "${link_path}"
	echo "  ${link_path#${SCRIPT_DIR}/} -> ${rel_target}"
}

main() {
	echo "创建 overlays 符号链接（源文件在各产品 *_app/ 目录）"
	rm -rf "${OVERLAYS}"
	mkdir -p "${OVERLAYS}"

	for product in "${!PRODUCT_APP[@]}"; do
		echo "[${product}]"
		for rel in ${PRODUCT_FILES[$product]}; do
			link_one "${product}" "${rel}"
		done
	done

	echo "完成。"
}

main "$@"
