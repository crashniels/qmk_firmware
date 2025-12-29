# Corsair K70 Vengeance RGB

* Keyboard Maintainer: [crashniels](https://github.com/crashniels)
* Hardware Supported: *K70 Vengeance RGB*, probably others as well as they are quite similar

Make example for this keyboard (after setting up your build environment):

    qmk compile -kb corsair/k70_vengeance_rgb -km default

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

# Specs K70 Vengeance RGB
MCU: LPC11U37F  
RGB: AN32181

# Specs Strafe RGB
MCU: LPC11U37F  
RGB: IS31FL3732

# How to Flash
* Press the button on the back or bottom of the keyboard while plugging it in
* CRP Disabled drive should show 
* Either backup or delete the original firmware.bin from there
* Copy the generated .bin file into the CRP Disabled drive. (Make sure to use Windows to copy the file as Linux has issues with the format for some reason)

      dd if=corsair_k70_vengeance_rgb_default.bin of=/path/to/firmware.bin bs=512 conv=notrunc oflag=direct,sync
