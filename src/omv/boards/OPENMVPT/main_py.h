static const char fresh_main_py[] =
    "import sensor, image, time, display, fir, math, pyb\n"
    "\n"
    "from machine import I2C\n"
    "from vl53l1x import VL53L1X\n"
    "\n"
    "# Color Tracking Thresholds (Grayscale Min, Grayscale Max)\n"
    "threshold_list = [(200, 255)]\n"
    "\n"
    "def main():\n"
    "\n"
    "    i2c = I2C(2)\n"
    "    # distance = VL53L1X(i2c)\n"
    "\n"
    "    sensor.reset()\n"
    "    sensor.set_pixformat(sensor.RGB565)\n"
    "    sensor.set_framesize(sensor.VGA)\n"
    "    time.sleep_ms(50)\n"
    "\n"
    "    fir.init(fir.FIR_LEPTON)\n"
    "    fir_img = sensor.alloc_extra_fb(fir.width(), fir.height(), sensor.GRAYSCALE)\n"
    "    time.sleep_ms(50)\n"
    "\n"
    "    lcd = display.RGBDisplay(framesize=display.FWVGA, refresh=60)\n"
    "\n"
    "    alpha_pal = image.Image(256, 1, sensor.GRAYSCALE)\n"
    "    for i in range(256): alpha_pal[i] = int(math.pow((i / 255), 2) * 255)\n"
    "\n"
    "    to_min = None\n"
    "    to_max = None\n"
    "\n"
    "    def map_g_to_temp(g):\n"
    "        return ((g * (to_max - to_min)) / 255.0) + to_min\n"
    "\n"
    "    while True:\n"
    "        img = sensor.snapshot()\n"
    "        # ta: Ambient temperature\n"
    "        # ir: Object temperatures (IR array)\n"
    "        # to_min: Minimum object temperature\n"
    "        # to_max: Maximum object temperature\n"
    "        ta, ir, to_min, to_max = fir.read_ir()\n"
    "\n"
    "        fir.draw_ir(fir_img, ir, color_palette = None)\n"
    "        fir_img_size = fir_img.width() * fir_img.height()\n"
    "\n"
    "        # Find IR Blobs\n"
    "        blobs = fir_img.find_blobs(threshold_list,\n"
    "                                   pixels_threshold = (fir_img_size // 100),\n"
    "                                   area_threshold = (fir_img_size // 100),\n"
    "                                   merge = True)\n"
    "\n"
    "        # Collect stats into a list of tuples\n"
    "        blob_stats = []\n"
    "        for b in blobs:\n"
    "            blob_stats.append((b.rect(), map_g_to_temp(fir_img.get_statistics(thresholds = threshold_list,\n"
    "                                                                              roi = b.rect()).mean())))\n"
    "        x_scale = img.width() / fir_img.width()\n"
    "        y_scale = img.height() / fir_img.height()\n"
    "        img.draw_image(fir_img, 0, 0, x_scale = x_scale, y_scale = y_scale,\n"
    "                       color_palette = sensor.PALETTE_IRONBOW,\n"
    "                       alpha_palette = alpha_pal,\n"
    "                       hint = image.BICUBIC)\n"
    "\n"
    "        # Draw stuff on the colored image\n"
    "        for b in blobs:\n"
    "            img.draw_rectangle(int(b.rect()[0] * x_scale), int(b.rect()[1] * y_scale),\n"
    "                               int(b.rect()[2] * x_scale), int(b.rect()[3] * y_scale))\n"
    "            img.draw_cross(int(b.cx() * x_scale), int(b.cy() * y_scale))\n"
    "        for blob_stat in blob_stats:\n"
    "            img.draw_string(int((blob_stat[0][0] * x_scale) + 4), int((blob_stat[0][1] * y_scale) + 1),\n"
    "                            '%.2f C' % blob_stat[1], mono_space = False, scale = 2)\n"
    "\n"
    "        # Draw ambient, min and max temperatures.\n"
    "        img.draw_string(4, 0, 'Lepton Temp: %0.2f C' % ta, color = (255, 255, 255), mono_space = False, scale = 2)\n"
    "        img.draw_string(4, 18, 'Min Temp: %0.2f C' % to_min, color = (255, 255, 255), mono_space = False, scale = 2)\n"
    "        img.draw_string(4, 36, 'Max Temp: %0.2f C' % to_max, color = (255, 255, 255), mono_space = False, scale = 2)\n"
    "        # img.draw_string(4, 54, 'Distance: %d mm' % distance.read(), color = (255, 255, 255), mono_space = False, scale = 2)\n"
    "\n"
    "        lcd.write(img, x_size=lcd.width(), hint=image.BILINEAR)\n"
    "\n"
    "try:\n"
    "    main()\n"
    "except OSError:\n"
    "\n"
    "    # I2C Bus may be stuck\n"
    "    p = pyb.Pin('P4', pyb.Pin.OUT_OD)\n"
    "    for i in range(20000):\n"
    "        p.value(not p.value())\n"
    "\n"
    "    pyb.hard_reset()\n"
;
