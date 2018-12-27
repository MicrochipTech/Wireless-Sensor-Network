@ECHO off
setlocal EnableDelayedExpansion

:: %1 type of bus
:: %2 type of layout (D21-SAMW25)
:: %3 (3400-2B0-3A0) 
:: %4 (aardvark_serial_number)
:: %5 com port for edbg usb device

SET FIRMWARE_2B0_PATH=../../../firmware/m2m_aio_2b0.bin		
SET FIRMWARE_3A0_PATH=../../../firmware/m2m_aio_3a0.bin		
SET FIRMWARE_3400_PATH=../../../firmware/m2m_aio_3400.bin

SET OTA_2B0=../../../ota_firmware/m2m_ota_2b0.bin
SET OTA_3A0=../../../ota_firmware/m2m_ota_3a0.bin
SET OTA_3400=../../../ota_firmware/m2m_ota_3400.bin

SET CA_DIR=../../../tls_cert_store/CA
SET TLS_RSA_KEY=../../../tls_cert_store/winc_rsa.key
SET TLS_RSA_CRT=../../../tls_cert_store/winc_rsa.cer
SET TLS_ECDSA_CRT=../../../tls_cert_store/winc_ecdsa.cer
SET TLS_SRV_KEY=../../../tls_cert_store/atwinc1500.key
SET TLS_SRV_CRT=../../../tls_cert_store/atwinc1500.cer

echo Mode %1
if "%1" == "UART"  Goto contine_UART
if "%1" == "I2C"   Goto contine_I2C
if "%1" == "OTA"   Goto contine_OTA

goto usage

:contine_I2C
Set  FMode=debug_i2c
set Mode=I2C
goto START

:contine_OTA
Set  FMode=debug_ota
set Mode=OTA
goto START

:contine_UART
Set  FMode=debug_uart
set Mode=UART
goto START

:START

if "a%4a" == "aa" (
	set AARDVARK=0
) else (
	set AARDVARK=%4
)

if "a%5a" == "aa" (
  set COMPORT=
) else (
  if "a%5a" == "a0a" (
  	set COMPORT=
  ) else (
   	set COMPORT=-port %5
  )
)

if "a%2a" == "aD21a" goto goodlayout
if "a%2a" == "aSAMD21a" goto goodlayout
if "a%2a" == "aSAMW25a" goto goodlayout
if "a%2a" == "aSAMG55a" goto goodlayout
if "a%2a" == "aSAMG53a" goto goodlayout
if "a%2a" == "aSAM4Sa" goto goodlayout
if "a%2a" == "aSAML21a" goto goodlayout
if "a%2a" == "aSAML22a" goto goodlayout
if "a%2a" == "aSAMR21a" goto goodlayout
if "a%2a" == "aSAMR30a" goto goodlayout
goto usage
:goodlayout



echo Chip %3
if "%3" == "3400"  Goto chip3400
if "%3" == "2B0"   Goto chip2B0
if "%3" == "3A0"   Goto chip3A0

:USAGE
echo Usage %0 (I2C-UART-OTA) (D21-SAMD21-SAMW25) (3400-2B0-3A0) (aardvark_serial_number) (comport or 0) (test or dev)
TIMEOUT /T 30 
exit /b 2

:chip3400
set VARIANT=3400
goto parmsok
:chip3A0
set VARIANT=3A0
goto parmsok
:chip2B0
set VARIANT=2B0
goto parmsok

:parmsok


IF %FMode%==debug_ota (
	echo Creating Image...
	pushd Tools\image_builder\debug\

	image_builder ^
		-no_wait ^
		-ota_img %OTA_2B0% ^
		-fw_path ../../../firmware/wifi_v111/ASIC_2B0/wifi_firmware.bin ^
		-op_path %FIRMWARE_2B0_PATH% ^
		-bf_bin  ../../../boot_firmware/release2B0/boot_firmware.bin ^
		-pf_bin  ../../../programmer_firmware/release2B0/programmer_firmware.bin ^
		-df_bin  ../../../downloader_firmware/release2B0/downloader_firmware.bin ^
		-ate_img ../../../ate_firmware/m2m_ate.bin 
	IF %ERRORLEVEL% NEQ  0 goto FAILED
	
	image_builder ^
		-1003A0 ^
		-no_wait ^
		-ota_img  %OTA_3A0% ^
		-fw_path ../../../firmware/wifi_v111/ASIC_3A0/wifi_firmware.bin ^
		-op_path %FIRMWARE_3A0_PATH% ^
		-bf_bin  ../../../boot_firmware/release3A0/boot_firmware.bin ^
		-pf_bin  ../../../programmer_firmware/release3A0/programmer_firmware.bin ^
		-df_bin  ../../../downloader_firmware/release3A0/downloader_firmware.bin ^
		-ate_img ../../../ate_firmware/m2m_ate.bin 
	IF %ERRORLEVEL% NEQ  0 goto FAILED
	
	image_builder ^
		-no_wait ^
		-ota_img  %OTA_3400% ^
		-fw_path ../../../firmware/wifi_v111/ASIC_3400/wifi_firmware.bin ^
		-op_path %FIRMWARE_3400_PATH% ^
		-bf_bin  ../../../boot_firmware/release3400/boot_firmware.bin ^
		-pf_bin  ../../../programmer_firmware/release3400/programmer_firmware.bin ^
		-df_bin  ../../../downloader_firmware/release3400/downloader_firmware.bin ^
		-bt_img ../../../ble/bt_firmware/fw.bin 
	IF %ERRORLEVEL% NEQ  0 goto FAILED

	popd
	goto SUCCESS
)

pushd Tools\image_downloader\%FMode%
echo Downloading Image... (pod %AARDVARK%) (comport %COMPORT%)

echo image_downloader.exe ^
	-no_wait ^
	-aardvark %AARDVARK% %COMPORT% ^
	-bf_bin  ../../../boot_firmware/release%VARIANT%/boot_firmware.bin ^
	-pf_bin  ../../../programmer_firmware/release%VARIANT%/programmer_firmware.bin ^
	-df_bin  ../../../downloader_firmware/release%VARIANT%/downloader_firmware.bin ^
	-fw2b0_path %FIRMWARE_2B0_PATH% ^
	-fw3a0_path %FIRMWARE_3A0_PATH% ^
	-fw3400_path %FIRMWARE_3400_PATH%
image_downloader.exe ^
	-no_wait ^
	-aardvark %AARDVARK% %COMPORT% ^
	-bf_bin  ../../../boot_firmware/release%VARIANT%/boot_firmware.bin ^
	-pf_bin  ../../../programmer_firmware/release%VARIANT%/programmer_firmware.bin ^
	-df_bin  ../../../downloader_firmware/release%VARIANT%/downloader_firmware.bin ^
	-fw2b0_path %FIRMWARE_2B0_PATH% ^
	-fw3a0_path %FIRMWARE_3A0_PATH% ^
	-fw3400_path %FIRMWARE_3400_PATH%
IF %ERRORLEVEL% NEQ  0 goto FAILED

popd
pushd Tools\gain_builder\%FMode%

echo Downloading Gain Values...

if "%2" == "SAMW25" ( 
	Set GAIN_FILE=-hp ../gain_sheets/samw25_gain_setting.csv
) else (
    if "%3" == "3400" (
    	Set GAIN_FILE=-gtable ../gain_sheets/atwinc1500_mr210pb_gain_setting.csv 
    ) else (
    	Set GAIN_FILE=-hp ../gain_sheets/atwinc1500_mr210pb_gain_setting.csv 
    )
)
if "%3" == "3400" (
    echo gain_builder.exe ^
    	%GAIN_FILE% ^
    	-aardvark %AARDVARK% %COMPORT% ^
    	-bf_bin  ../../../boot_firmware/release%VARIANT%/boot_firmware.bin ^
    	-pf_bin  ../../../programmer_firmware/release%VARIANT%/programmer_firmware.bin ^
    	-df_bin  ../../../downloader_firmware/release%VARIANT%/downloader_firmware.bin ^
    	-no_wait
    gain_builder.exe ^
    	%GAIN_FILE% ^
    	-aardvark %AARDVARK% %COMPORT% ^
    	-bf_bin  ../../../boot_firmware/release%VARIANT%/boot_firmware.bin ^
    	-pf_bin  ../../../programmer_firmware/release%VARIANT%/programmer_firmware.bin ^
    	-df_bin  ../../../downloader_firmware/release%VARIANT%/downloader_firmware.bin ^
    	-no_wait
) else (
    echo gain_builder.exe ^
    	%GAIN_FILE% %COMPORT% ^
    	-bf_bin  ../../../boot_firmware/release%VARIANT%/boot_firmware.bin ^
    	-pf_bin  ../../../programmer_firmware/release%VARIANT%/programmer_firmware.bin ^
    	-df_bin  ../../../downloader_firmware/release%VARIANT%/downloader_firmware.bin ^
    	-no_wait
    gain_builder.exe ^
    	%GAIN_FILE% %COMPORT% ^
    	-bf_bin  ../../../boot_firmware/release%VARIANT%/boot_firmware.bin ^
    	-pf_bin  ../../../programmer_firmware/release%VARIANT%/programmer_firmware.bin ^
    	-df_bin  ../../../downloader_firmware/release%VARIANT%/downloader_firmware.bin ^
    	-no_wait
)
IF %ERRORLEVEL% NEQ  0 goto FAILED
popd

if a%VARIANT%a == a3400a (
    pushd Tools\tls_cert_flash_tool\%FMode%

    echo Downloading WINC TLS Certificate Materials...
	echo tls_cert_flash_tool.exe ^
		-aardvark %AARDVARK% %COMPORT% ^
		-bf_bin  ../../../boot_firmware/release%VARIANT%/boot_firmware.bin ^
		-pf_bin  ../../../programmer_firmware/release%VARIANT%/programmer_firmware.bin ^
		-df_bin  ../../../downloader_firmware/release%VARIANT%/downloader_firmware.bin ^
		-key %TLS_SRV_KEY% ^
		-cert %TLS_SRV_CRT%
	tls_cert_flash_tool.exe ^
		-aardvark %AARDVARK% %COMPORT% ^
		-bf_bin  ../../../boot_firmware/release%VARIANT%/boot_firmware.bin ^
		-pf_bin  ../../../programmer_firmware/release%VARIANT%/programmer_firmware.bin ^
		-df_bin  ../../../downloader_firmware/release%VARIANT%/downloader_firmware.bin ^
		-key %TLS_SRV_KEY% ^
		-cert %TLS_SRV_CRT%
	IF %ERRORLEVEL% NEQ  0 goto FAILED
    popd
) else (
    pushd Tools\tls_cert_flash_tool\%FMode%

    echo Downloading WINC TLS Certificate Materials...
    tls_cert_flash_tool.exe ^
        WRITE ^
        -key %TLS_RSA_KEY% %COMPORT% ^
        -cert %TLS_RSA_CRT% ^
        -cadir %CA_DIR% ^
        -erase ^
        -nowait
	IF %ERRORLEVEL% NEQ  0 goto FAILED
    tls_cert_flash_tool.exe ^
        WRITE ^
        -nokey %COMPORT% ^
        -cert %TLS_ECDSA_CRT% ^
        -cadir %CA_DIR% ^
        -nowait
	IF %ERRORLEVEL% NEQ  0 goto FAILED
    popd
)

pushd Tools\root_certificate_downloader\%FMode%

echo Downloading root certificates...
set /a c = 0
set seq=
for %%X in (..\binary\*.cer) do (
	set /a c+=1
	@set seq=!seq! %%X
)
echo root_certificate_downloader.exe ^
	-n %c% %seq% ^
	-no_wait ^
	-aardvark %AARDVARK% %COMPORT% ^
	-bf_bin  ../../../boot_firmware/release%VARIANT%/boot_firmware.bin ^
	-pf_bin  ../../../programmer_firmware/release%VARIANT%/programmer_firmware.bin ^
	-df_bin  ../../../downloader_firmware/release%VARIANT%/downloader_firmware.bin ^
	-e
root_certificate_downloader.exe ^
	-n %c% %seq% ^
	-no_wait ^
	-aardvark %AARDVARK% %COMPORT% ^
	-bf_bin  ../../../boot_firmware/release%VARIANT%/boot_firmware.bin ^
	-pf_bin  ../../../programmer_firmware/release%VARIANT%/programmer_firmware.bin ^
	-df_bin  ../../../downloader_firmware/release%VARIANT%/downloader_firmware.bin ^
	-e
IF %ERRORLEVEL% NEQ  0 goto FAILED
popd

goto SUCCESS

:FAILED
popd
echo     #######################################################################
echo     ##                                                                   ##
echo     ##                    ########    ###     ####  ##                   ##
echo     ##                    ##         ## ##     ##   ##                   ##
echo     ##                    ##        ##   ##    ##   ##                   ##
echo     ##                    ######   ##     ##   ##   ##                   ##
echo     ##                    ##       #########   ##   ##                   ##
echo     ##                    ##       ##     ##   ##   ##                   ##
echo     ##                    ##       ##     ##  ####  ########             ##
echo     ##                                                                   ##
echo     ####################################################################### 
pause
exit /b 1

:SUCCESS

echo OK
echo     #######################################################################
echo     ##                                                                   ##
echo     ##                 ########     ###     ######   ######              ##
echo     ##                 ##     ##   ## ##   ##    ## ##    ##             ##
echo     ##                 ##     ##  ##   ##  ##       ##                   ##
echo     ##                 ########  ##     ##  ######   ######              ##
echo     ##                 ##        #########       ##       ##             ##
echo     ##                 ##        ##     ## ##    ## ##    ##             ##
echo     ##                 ##        ##     ##  ######   ######              ##
echo     ##                                                                   ##
echo     #######################################################################

echo Downloading ends successfully
pause
