from arduino import grove_mlx9064x
from machine import LCD
from machine import Sprite
import time

tft = LCD()
mlx90641 = grove_mlx9064x()
mlx90641.set_refresh_rate(mlx90641.rate._16HZ)
spr = Sprite(tft) #create a buff to show IR data more fast


MinTemp = 25
MaxTemp = 38

red = 0
green = 0
blue = 0

a = 0.0
b = 0.0
c = 0.0
d = 0.0

ShowGrid = -1
pixels = [0.0]*64
colors_table = [0]*100

XPixels = 40
YPixels = 40

HDTemp = [0.0] * XPixels * YPixels

def toggleGrid():
    global ShowGrid
    ShowGrid = ShowGrid * -1
    tft.fillRect(15, 15, 210, 210, tft.color.BLACK)

def constrain(amt,low,high):
    value = low if (amt < low) else (high if (amt > high) else amt)
    return int(value)

#function to get the cutoff points in the temp vs RGB graph
def Getabcd():
    global a
    global b
    global c
    global d
    a = MinTemp + (MaxTemp - MinTemp) * 0.2121
    b = MinTemp + (MaxTemp - MinTemp) * 0.3182
    c = MinTemp + (MaxTemp - MinTemp) * 0.4242
    d = MinTemp + (MaxTemp - MinTemp) * 0.8182

#function to get color table
def GetColors():

    global colors_table

    val = 25.0
    for i in range(100):
        colors_table[i] = calcColor(val)
        val += 0.1

    #print(colors_table)

#get colort form color_table
def GetColor(val):
    if val <= 25.0:
        return 30
    if val >= 35.0:
        return 65518
    index = int((val - 25.0) * 10)
    return colors_table[index]
    
# my fast yet effective color interpolation routine
def calcColor(val):
    
    global red
    global green
    global blue

    red = constrain(255.0 / (c - b) * val - ((b * 255.0) / (c - b)), 0, 255)

    if (val > MinTemp) and (val < a):
        green = constrain(255.0 / (a - MinTemp) * val - (255.0 * MinTemp) / (a - MinTemp), 0, 255)
    elif (val >= a) & (val <= c):
        green = 255
    elif (val > c):
        green = 0

    if val <= b:
        blue = constrain(255.0 / (a - b) * val - (255.0 * b) / (a - b), 0, 255)
    elif ((val > b) and (val <= d)):
        blue = 0;
    elif (val > d):
        blue = constrain(240.0 / (MaxTemp - d) * val - (d * 240.0) / (MaxTemp - d), 0, 240)
    
    return tft.color565(red, green, blue)

#function to draw a legend
def DrawLegend():
    #color legend with max and min text
  
    inc = (MaxTemp - MinTemp ) / 160.0
    j = 0
    ii = MinTemp
    while ii < MaxTemp:
        tft.drawFastHLine(260, 200 - j, 30, GetColor(ii))
        j = j + 1
        ii = ii + inc
    

    tft.setTextSize(2)
    tft.setTextColor(tft.color.WHITE, tft.color.BLACK)
    str_tmep = '{a}/{b}'.format(a=MaxTemp, b=95)
    tft.drawString(str_tmep, 245, 20)
    str_tmep = '{a}/{b}'.format(a=MinTemp, b=77)
    tft.drawString(str_tmep, 245, 210)

# function to display the results
def DisplayGradient():

    row = 0
    for col in range(XPixels * YPixels):
        spr.fillRect((row * 5) + 10 , (col % YPixels * 5) + 10 , 5, 5, GetColor(HDTemp[row*YPixels+col%YPixels]))  
        if not (col + 1) % YPixels:
            row = row + 1   

   
def init():
    global pixels

    print("\n\r")
    tft.fillScreen(tft.color.BLACK)
    tft.setRotation(3)
    spr.createSprite(220,220)
   
    Getabcd()
    GetColors()
    DrawLegend()

def run():
    
    global pixels
    global HDTemp
    
    tick1 = time.ticks_ms()

    while True:

        spr.fillSprite(spr.color.WHITE)
        #get a 40*40 list of temperature data
        HDTemp = mlx90641.get_frame_data()
        # #return
        DisplayGradient()
        #Crosshair in the middle of the screen
        spr.drawCircle(115, 115, 5, tft.color.WHITE)
        spr.drawFastVLine(115, 105, 20, tft.color.WHITE)
        spr.drawFastHLine(105, 115, 20, tft.color.WHITE)
        spr.setRotation(0)
        spr.setTextColor(tft.color.WHITE)
        tft.setRotation(2)
        spr.pushSprite(0,0)
        tft.setRotation(3)
        tft.drawFloat(HDTemp[20*40+20], 2, 90, 20)



def main():
    init()
    run()


if __name__ == "__main__":
    main()