#!/usr/bin/env bash

set -euo pipefail

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)
root=$script_dir
show_all=false

while (($#)); do
    case $1 in
        -Root|--root)
            if (($# < 2)); then
                printf '缺少 -Root 的目录参数。\n' >&2
                exit 2
            fi
            root=$2
            shift 2
            ;;
        -All|--all)
            show_all=true
            shift
            ;;
        -h|--help)
            printf '用法：%s [--root 目录] [--all]\n默认列出逾期、今天和明天的复习笔记。\n' "${0##*/}"
            exit 0
            ;;
        *)
            printf '未知参数：%s\n用法：%s [-Root 目录] [-All]\n' "$1" "${0##*/}" >&2
            exit 2
            ;;
    esac
done

if ! root=$(cd -- "$root" 2>/dev/null && pwd -P); then
    printf '目录不存在或无法访问。\n' >&2
    exit 1
fi

today=$(date +%F)
if [[ $(date -d '2000-01-02' +%F 2>/dev/null || :) == '2000-01-02' ]]; then
    date_flavor=gnu
    tomorrow=$(date -d "$today + 1 day" +%F)
elif tomorrow=$(date -v+1d +%F 2>/dev/null); then
    date_flavor=bsd
else
    printf '无法计算日期：需要 GNU date 或 BSD date。\n' >&2
    exit 1
fi

valid_date() {
    local raw=$1 parsed
    if [[ $date_flavor == gnu ]]; then
        parsed=$(date -d "$raw" +%F 2>/dev/null) || return 1
    else
        parsed=$(date -j -f '%Y-%m-%d' "$raw" +%F 2>/dev/null) || return 1
    fi
    [[ $parsed == "$raw" ]]
}
items=()
invalid=()

while IFS= read -r -d '' file; do
    # Frontmatter must be the first thing in the file.
    if ! IFS= read -r line < "$file"; then
        continue
    fi
    line=${line%$'\r'}
    [[ $line == '---' ]] || continue

    due_found=false
    stage_found=false
    due_value=''
    stage_value=''
    closed=false
    first_line=true
    while IFS= read -r line || [[ -n $line ]]; do
        if $first_line; then
            first_line=false
            continue
        fi
        line=${line%$'\r'}
        if [[ $line == '---' ]]; then
            closed=true
            break
        fi
        if ! $due_found && [[ $line =~ ^review_due:[[:blank:]]*([^#]*) ]]; then
            due_found=true
            due_value=${BASH_REMATCH[1]}
        fi
        if ! $stage_found && [[ $line =~ ^study_stage:[[:space:]]*([^#]+) ]]; then
            stage_found=true
            stage_value=${BASH_REMATCH[1]}
        fi
    done < "$file"

    if ! $closed || ! $due_found; then
        continue
    fi
    # PowerShell's IsNullOrWhiteSpace checks the original field before quote removal.
    [[ $due_value =~ [^[:space:]] ]] || continue

    # Trim whitespace, followed by any single or double quotes at either end.
    raw_date=$(printf '%s' "$due_value" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//' -e "s/^[\"']*//" -e "s/[\"']*$//")

    name=${file##*/}
    if [[ ! $raw_date =~ ^[0-9]{4}-[0-9]{2}-[0-9]{2}$ ]] || ! valid_date "$raw_date"; then
        invalid+=("$name"$'\t'"$raw_date")
        continue
    fi

    stage_value=$(printf '%s' "$stage_value" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')
    stage_value=$(printf '%s' "$stage_value" | tr '[:lower:]' '[:upper:]')
    case $stage_value in
        LEARN) stage=LEARN ;;
        D1|D3|D7|D14|D30|D60) stage=$stage_value ;;
        *) stage=TASK ;;
    esac

    relative_path=${file#"$root"/}
    items+=("$raw_date"$'\t'"$stage"$'\t'"$relative_path")
done < <(find "$root" \
    \( -type d \( -name .git -o -name .obsidian -o -name .trash \) -prune \) -o \
    \( -type f -name '*.md' -print0 \) 2>/dev/null)

display=()
for item in "${items[@]}"; do
    due=${item%%$'\t'*}
    if $show_all || [[ $due < $tomorrow || $due == "$tomorrow" ]]; then
        display+=("$item")
    fi
done

if ((${#display[@]} == 0)); then
    printf '没有符合条件的复习笔记。\n'
else
    if $show_all; then mode=ALL; else mode=NEAR_TERM; fi
    printf '[ REVIEW_DUE | MODE: %s | TODAY: %s | COUNT: %d ]\n' \
        "$mode" "$today" "${#display[@]}"

    while IFS= read -r item; do
        due=${item%%$'\t'*}
        rest=${item#*$'\t'}
        stage=${rest%%$'\t'*}
        name=${rest#*$'\t'}
        if [[ $due < $today ]]; then
            label=OVERDUE
        elif [[ $due == "$today" ]]; then
            label=TODAY
        else
            label=FUTURE
        fi
        printf -v label_padding '%*s' "$((7 - ${#label}))" ''
        printf -- '- [%s]%s %s | %-7s | %s\n' \
            "$label" "$label_padding" "$due" "[$stage]" "$name"
    done < <(printf '%s\n' "${display[@]}" | LC_ALL=C sort -t $'\t' -k1,1 -k3,3)
fi

if ((${#invalid[@]} > 0)); then
    printf 'WARNING: 以下 review_due 不是 yyyy-MM-dd，已跳过：\n' >&2
    while IFS= read -r item; do
        name=${item%%$'\t'*}
        value=${item#*$'\t'}
        printf 'WARNING: - %s  (review_due: %s)\n' "$name" "$value" >&2
    done < <(printf '%s\n' "${invalid[@]}" | LC_ALL=C sort -t $'\t' -k1,1)
fi
