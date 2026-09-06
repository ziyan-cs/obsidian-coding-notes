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

        $items.Add([pscustomobject]@{
            Due  = $parsedDate.Date
            Name = $file.Name
        })
    }

Write-Output ''

$displayItems = @()
if ($All) {
    $displayItems = $items | Sort-Object Due, Name
}
else {
    $dueNow = @($items | Where-Object { $_.Due -le $today } | Sort-Object Due, Name)
    if ($dueNow.Count -gt 0) {
        $displayItems = $dueNow
    }
    elseif ($items.Count -gt 0) {
        $nextDate = ($items | Sort-Object Due | Select-Object -First 1).Due
        $displayItems = $items | Where-Object { $_.Due -eq $nextDate } | Sort-Object Name
    }
}

if ($displayItems.Count -eq 0) {
    Write-Output '没有到期或待复习的笔记。'
}
else {
    $mode = if ($All) { 'ALL' } elseif (($displayItems | Select-Object -First 1).Due -gt $today) { 'NEXT' } else { 'DUE' }
    Write-Output ("[ REVIEW_DUE | MODE: {0} | TODAY: {1:yyyy-MM-dd} | COUNT: {2} ]" -f $mode, $today, $displayItems.Count)

    foreach ($item in $displayItems) {
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
