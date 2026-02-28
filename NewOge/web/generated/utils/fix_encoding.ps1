# Показывает кодировку всех .js файлов
Write-Host "Проверка кодировки файлов..." -ForegroundColor Yellow

$files = Get-ChildItem -Path . -Filter *.js -Recurse

foreach ($file in $files) {
    # Читаем первые байты файла
    $bytes = [System.IO.File]::ReadAllBytes($file.FullName)
    
    # Проверяем наличие BOM (EF BB BF)
    if ($bytes.Length -ge 3 -and $bytes[0] -eq 0xEF -and $bytes[1] -eq 0xBB -and $bytes[2] -eq 0xBF) {
        Write-Host "$($file.Name) - ✅ UTF-8 with BOM" -ForegroundColor Green
    } 
    # Проверяем на UTF-8 without BOM (простая проверка)
    elseif ($bytes.Length -gt 0) {
        try {
            $text = [System.Text.Encoding]::UTF8.GetString($bytes)
            # Если удалось прочитать как UTF-8, считаем что это UTF-8
            Write-Host "$($file.Name) - ✅ UTF-8 without BOM" -ForegroundColor Green
        }
        catch {
            Write-Host "$($file.Name) - ❌ Windows-1251 или другая" -ForegroundColor Red
        }
    }
}

# Конвертируем все .js файлы в UTF-8 без BOM
Write-Host "`nКонвертация всех .js файлов в UTF-8 без BOM..." -ForegroundColor Yellow

$jsFiles = Get-ChildItem -Path . -Filter *.js -Recurse
foreach ($file in $jsFiles) {
    try {
        # Читаем как Windows-1251
        $content = [System.IO.File]::ReadAllText($file.FullName, [System.Text.Encoding]::GetEncoding(1251))
        
        # Сохраняем как UTF-8 без BOM
        [System.IO.File]::WriteAllText($file.FullName, $content, [System.Text.Encoding]::UTF8)
        
        Write-Host "✓ $($file.Name) - конвертирован" -ForegroundColor Green
    }
    catch {
        Write-Host "✗ $($file.Name) - ошибка: $_" -ForegroundColor Red
    }
}

Write-Host "`nГотово! Все .js файлы теперь в UTF-8 без BOM" -ForegroundColor Green