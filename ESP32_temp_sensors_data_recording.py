import serial
import time
import json
from itertools import chain
import numpy as np
import pyqtgraph as pg
import urllib.request
import cv2
from datetime import datetime

# Serial port parameters
serial_speed = 115200
# ser_32 = serial.Serial('/dev/cu.usbserial-4',baudrate=serial_speed,timeout=1)
# ser_8266 = serial.Serial('/dev/cu.usbserial-0001',baudrate=serial_speed,timeout=1)
# ser_E = serial.Serial('/dev/tty.usbserial-1420',baudrate=serial_speed,timeout=1)
ser_32 = serial.Serial('COM15',baudrate=serial_speed,timeout=1)
ser_8266 = serial.Serial('COM4',baudrate=serial_speed,timeout=1)
ser_E = serial.Serial('COM5',baudrate=9600,timeout=1)

######################################ESP-Cam
font = cv2.FONT_HERSHEY_SIMPLEX
bottomLeftCornerOfText = (10,600)
fontScale = 1
fontColor = (0,0,0)
thickness = 2
lineType = 2

url = 'http://192.168.0.245/capture'

# Calibration left thermometer
#temperature value 1
t1_F1 = 80
#pixel location of temperature in y axis 1
t1_P1 = 457
#temperature value 2
t1_F2 = 60
#pixel location of temperature in y axis 2
t1_P2 = 958

# Calibration right thermometer (if used)
t2_F1 = 80
t2_P1 = 416
t2_F2 = 60
t2_P2 = 934



img = cv2.imread("C:/Users/Damian/Desktop/blue.jpg")
#img = cv2.imread("/Users/damianwilliams/Desktop/blue.jpg")
hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
mu, sig = cv2.meanStdDev(hsv)

#--------------------------##------------Range of blue_values______________##

a = 7
#--------------------------##----------------------------------------------##


#--------------------------##------------threshold for rectangle size defined as liquid column size (ignores small
# area)______________##

minArea = 2000
#--------------------------##-------------------------------------------------------------------------------------------------------------##


change_resolution= urllib.request.urlopen(url, timeout=5)
time.sleep(2)
change_resolution.close()

def CalculateTemp(temp1, temp2, pixel1, pixel2, yval):
    pix_per_deg = (pixel2 - pixel1) / (temp2 - temp1)
    pixels_diff = yval - pixel2
    delta_fah = pixels_diff / pix_per_deg
    fahrenheit = temp2 + delta_fah
    celcius = ((fahrenheit - 32) * (5 / 9))
    temps = [celcius,fahrenheit]
    return temps



idx = 0


###############################################

data_2d = np.zeros((50,46))


win = pg.GraphicsLayoutWidget(show=True)
win.setWindowTitle('pyqtgraph example: Scrolling Plots')
p2 = win.addPlot()
p2.setYRange(20, 30, padding=0)
p2.setMouseEnabled(x=None,y=None)
p2.setLabel(axis='left', text='Temperature (C)')
p2.setLabel(axis='bottom', text='Point number')
p2.hideAxis('bottom')
ptr1 = 0

curves_2d = [p2.plot(pen=idx) for idx, n in enumerate(data_2d.T)]



def plot_data(all_sensors):
    global data_2d
    array_all_sensors = np.array(all_sensors)
    print(array_all_sensors)
    data_2d = np.vstack((data_2d, array_all_sensors))
    print("*****")
    data_2d = np.delete(data_2d, (0), axis=0)


    for idx, n in enumerate(data_2d.T):
        # print(enumerate(data_2d))
        # print(n)
        plot_yes = (data_2d[:, idx])
        # print(plot_yes)
        # print("***")
        # plot_yes = (data_2d[:, idx])
        curves_2d[idx].setData(plot_yes)

    pg.QtWidgets.QApplication.processEvents()


if ser_32.isOpen() and ser_8266.isOpen() and ser_E.isOpen():
#if ser_32.isOpen() and ser_8266.isOpen():

    print("fd")

    while True:
        ser_32.write(b'R')
        ser_8266.write(b'r')
        ser_E.write(b'T\n\r')
        time.sleep(1)



        try:

            imgResp = urllib.request.urlopen(url, timeout=10)
            print(imgResp)
            imgNp = np.array(bytearray(imgResp.read()), dtype=np.uint8)
            img = cv2.imdecode(imgNp, -1)
            # img = cv2.flip(img, 0)
            # img = cv2.flip(img, 1)
            #cv2.imshow("Image used for detection", img)

            Image_for_detection = img
            Thermometer_image = img

            height_image, width_image, channels_image = Image_for_detection.shape
            print(f'image dimensions (pixels):, height =  {height_image}, width = {width_image}')

            # Mask for blue color
            therm_hsv = cv2.cvtColor(Image_for_detection, cv2.COLOR_BGR2HSV)
            therm_hsv = cv2.inRange(therm_hsv, mu - a * sig, mu + a * sig)

            # --------------------------##------------Parameters that will fill any small gaps in the color mask that
            # encompasses the thermometer-------##

            kernel = np.ones((3, 3), np.uint8)
            dilated_image = cv2.dilate(therm_hsv, kernel, iterations=1)

            cv2.imshow("Image used for detection", dilated_image)

            # --------------------------##----------------------------------------------##

            mask_image = cv2.cvtColor(dilated_image, cv2.COLOR_GRAY2RGB)
            contours, hierarchy = cv2.findContours(image=dilated_image, mode=cv2.RETR_TREE,
                                                   method=cv2.CHAIN_APPROX_NONE)

            number_regions_found = 0

            for cnt in contours:
                x, y, w, h = cv2.boundingRect(cnt)
                area = cv2.contourArea(cnt)

                if area > minArea:

                    # defines area where left thermometer would be found
                    if x < 1194:

                        ##########################################################################################
                        Thermometer_image = cv2.rectangle(Thermometer_image, (x, y), (x + w, y + h), (0, 255, 0), 1)
                        mask_image = cv2.rectangle(mask_image, (x, y), (x + w, y + h), (0, 255, 0), 1)
                        ##########################################################################################

                        therm_reading_1 = CalculateTemp(t1_F1, t1_F2, t1_P1, t1_P2, y)
                        therm_reading_1_float = therm_reading_1[0]
                        therm_1_stringC = "%.1f" % round(therm_reading_1[0], 1)
                        #print("Therm 1 Calculated_value: " + therm_1_stringC)
                        therm_1_stringF = "%.1f" % round(therm_reading_1[1], 1)
                        #print("Therm 1 Calculated_value: " + therm_1_stringF)
                        number_regions_found = 1

                    else:
                        #################################################################################################
                        Thermometer_image = cv2.rectangle(Thermometer_image, (x, y), (x + w, y + h), (0, 0, 255), 1)
                        mask_image = cv2.rectangle(mask_image, (x, y), (x + w, y + h), (0, 0, 255), 1)
                        #################################################################################################

                        therm_reading_2 = CalculateTemp(t2_F1, t2_F2, t2_P1, t2_P2, y)
                        therm_2_stringC = "%.1f" % round(therm_reading_2[0], 1)
                        therm_reading_2_float = therm_reading_2[0]
                        print("Therm 2 Calculated_value C: " + therm_2_stringC)
                        therm_2_stringF = "%.1f" % round(therm_reading_2[1], 1)
                        print("Therm 2 Calculated_value C: " + therm_2_stringF)

                        number_regions_found = number_regions_found + 1

                    if (number_regions_found == 2):

                        vis = np.concatenate((mask_image, Thermometer_image), axis=1)

                        #######################################################################
                        #cv2.imshow("Image used for detection", vis)
                        #######################################################################

                        # Save data in csv file

                        now = datetime.now()
                        dt_string = now.strftime("%Y-%m-%d %H:%M:%S")
                        current_time = time.time()

                        #for_log = [current_time, dt_string, therm_1_stringC, therm_2_stringC, therm_1_stringF,
                          #         therm_2_stringF]

                        thermo_dictonary = {"Thermometers": [therm_reading_1_float,therm_reading_2_float]}
                        json_thermo = json.dumps(thermo_dictonary)
                        print(f'Thermometer_first {json_thermo}')

                        #######################################################################
                        #with open("C:/Users/Damian/Desktop/test_data.csv", "a") as f:
                         #   writer = csv.writer(f, delimiter=",")
                           # writer.writerow(for_log)
                        ######################################################################

                        # Create image with annotation
                        place = (f"{dt_string} \n"
                                 f"{therm_1_stringF} F, {therm_1_stringC} C\n"
                                 f"{therm_2_stringF} F, {therm_2_stringC} C\n")

                        position = (30, 30)
                        text = place
                        font_scale = 0.75
                        color = (255, 0, 0)
                        thickness = 2
                        font = cv2.FONT_HERSHEY_SIMPLEX
                        line_type = cv2.LINE_AA
                        text_size, _ = cv2.getTextSize(text, font, font_scale, thickness)
                        line_height = text_size[1] + 5
                        x, y0 = position
                        for i, line in enumerate(text.split("\n")):
                            y = y0 + i * line_height
                            img = cv2.putText(img,
                                              line,
                                              (x, y),
                                              font,
                                              font_scale,
                                              color,
                                              thickness,
                                              line_type)

                        #######################################################################################

                        #Save_name = "C:/Users/Damian/Desktop/Thermometer_images/thermometer_" + str(idx) + ".jpg"
                        #Save_name = "/Users/damianwilliams/Desktop/Thermometer_images/thermometer_" + str(idx) + ".jpg"
                        #cv2.imwrite(Save_name, img)
                        #cv2.imshow('Thermometer_images', img)
                        ########################################################################################

                        print(idx)
                        idx = idx + 1





            serial_data_32 = ser_32.readline()
            print(f'serial_data_32 {serial_data_32}')
            decoded_32 = serial_data_32.decode("utf-8")
            print(f'decoded_32 {decoded_32}')
            json_32 = json.loads(decoded_32)
            values_32 = list(json_32.values())
            list_32 = list(chain.from_iterable(values_32))

            serial_data_8266 = ser_8266.readline()
            print(f'serial_data_8266 {serial_data_8266}')
            decoded_8266 = serial_data_8266.decode("utf-8")
            print(f'decoded_8266 {serial_data_8266}')
            json_8266 = json.loads(decoded_8266)
            values_8266 = list(json_8266.values())
            list_8266 = list(chain.from_iterable(values_8266))




            #eutronics
            eutronics_temp = ser_E.readline()
            ser_E.flushInput()
            ser_E.flushOutput()



            temp = eutronics_temp.decode("utf-8")
            print(f'Eutronics {temp}')

            temp = float(temp)
            print(f'Eutronics {temp}')
            eut_dict = {"Eutronics": [temp]}
            print(f'Eutronics {eut_dict}')
            json_eut = json.dumps(eut_dict)

            all_sensors = list_32 + list_8266

            print(f'Thermometer {json_thermo}')

            jsonMerged = {**json.loads(decoded_8266), **json.loads(decoded_32), **json.loads(json_eut),**json.loads(json_thermo)}
            merged_data_json = json.dumps(jsonMerged)

            #merged_data_json = {key: value for (key, value) in (json_32.items() + json_8266.items())}
            print(merged_data_json)

            #print(all_sensors)

            #save_name = "/Users/damianwilliams/Desktop/" + str(time.time_ns() // 1000000) + ".json"
            save_name = "C:/Users/Damian/Desktop/json_sensor/" + str(time.time_ns() // 1000000) + ".json"
            with open(save_name, 'w') as json_file:
                json.dump(merged_data_json, json_file)



            try:
                plot_data(all_sensors)
                print("DDDDD")




            except:
                print("String could not be converted to JSON")


        except Exception as e:
            print(e)
            #print("invalid")
        # if(len(line)>0):
        #   esp8266_json = json.loads(line)
        #   print(type(esp8266_json))
        cv2.destroyAllWindows()

#   print(esp8266_json)
#           # print(esp8266_json["TMP102_esp8266"])
#           #list_8266 = list(chain.from_iterable(esp8266_json))
#           values_8266 = list(esp8266_json.values())
#           list_8266 = list(chain.from_iterable(values_8266))
#           list_to_array_8266 = np.array(list_8266)
#           print(list_to_array_8266)
#           data_2d = np.vstack((data_2d, list_8266))
#           print("*****")
#           data_2d = np.delete(data_2d, (0), axis=0)
#
#           #print(data_2d)
#         try:
#
#             for idx, n in enumerate(data_2d.T):
#                 #print(enumerate(data_2d))
#                 #print(n)
#                 plot_yes = (data_2d[:,idx])
#                 #print(plot_yes)
#                 #print("***")
#                 #plot_yes = (data_2d[:, idx])
#                 curves_2d[idx].setData(plot_yes)
#                 # A[:,n]
#                 # n[:-1] = n[1:]
#                 #
#                 # n[-1] = data_2d[idx]
#                 #
#                 #
#                 # #print(idx)
#
#
#             pg.QtWidgets.QApplication.processEvents()
#
#
#
#         except Exception as err:
#                 print(Exception, err)
#                 #ser.close()
#
# data = ser.readline()
# data = data.decode('utf_8')
# print(data)
