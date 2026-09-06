[CmdletBinding()]
param(
    [string]$Root = $PSScriptRoot,
    [switch]$All
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$resolvedRoot = (Resolve-Path -LiteralPath $Root).Path
$today = (Get-Date).Date
$items = [System.Collections.Generic.List[object]]::new()
$invalid = [System.Collections.Generic.List[object]]::new()

Get-ChildItem -LiteralPath $resolvedRoot -Recurse -File -Filter '*.md' |
    Where-Object {
        $_.FullName -notmatch '\\(?:\.git|\.obsidian|\.trash)\\'
    } |
    ForEach-Object {
        $file = $_
        $content = [System.IO.File]::ReadAllText($file.FullName, [System.Text.Encoding]::UTF8)

        # Only accept review_due from YAML frontmatter at the start of a note.
        $frontmatter = [regex]::Match($content, '(?s)\A---\r?\n(.*?)\r?\n---')
        if (-not $frontmatter.Success) {
            return
        }

        $due = [regex]::Match($frontmatter.Groups[1].Value, '(?m)^review_due:\s*([^#\r\n]+)')
        if (-not $due.Success) {
            return
        }

        $rawDate = $due.Groups[1].Value.Trim().Trim('"', "'")
        $parsedDate = [datetime]::MinValue
        if (-not [datetime]::TryParseExact(
                $rawDate,
                'yyyy-MM-dd',
                [System.Globalization.CultureInfo]::InvariantCulture,
                [System.Globalization.DateTimeStyles]::None,
                [ref]$parsedDate
            )) {
            $invalid.Add([pscustomobject]@{
                Name = $file.Name
                Value = $rawDate
            })
            return
        }

        if ($All -or $parsedDate.Date -le $today) {
            $items.Add([pscustomobject]@{
                Due  = $parsedDate.Date
                Name = $file.Name
            })
        }
    }

Write-Output ''

if ($items.Count -eq 0) {
    Write-Output '没有到期或待复习的笔记。'
}
else {
    Write-Output ("[ REVIEW_DUE | TODAY: {0:yyyy-MM-dd} | COUNT: {1} ]" -f $today, $items.Count)

    foreach ($item in ($items | Sort-Object Due, Name)) {
        $label = if ($item.Due -lt $today) {
            'OVERDUE'
        }
        elseif ($item.Due -eq $today) {
            'TODAY'
        }
        else {
            'FUTURE'
        }
        Write-Output ("- [{0}] {1:yyyy-MM-dd} | {2}" -f $label, $item.Due, $item.Name)
    }
}

if ($invalid.Count -gt 0) {
    Write-Warning '以下 review_due 不是 yyyy-MM-dd，已跳过：'
    foreach ($item in ($invalid | Sort-Object Name)) {
        Write-Warning ("- {0}  (review_due: {1})" -f $item.Name, $item.Value)
    }
}

Write-Output ''
