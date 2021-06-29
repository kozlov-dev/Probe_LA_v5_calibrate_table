#[-5:12:0,2]
#[-5:27:0,2]
# x0 -5000 | y0 0
# x1 12000 | y1 85
# length_mode_12v = int(((-1)*MIN_VOLT_MODE_12+MAX_VOLT_MODE_12)/step_calibrate)



MIN_VOLT_MODE_12 = -9360
MAX_VOLT_MODE_12 = 9360
STEP_CALIBRATE = 1040
volt =2400


# MIN_VOLT_MODE_27 = -27000
# MAX_VOLT_MODE_27 = 27000
# length_mode_27v = int(((-1)*MIN_VOLT_MODE_27+MAX_VOLT_MODE_27)/step_calibrate)
#---------------------------------------
minVolt = MIN_VOLT_MODE_12
maxVolt = MAX_VOLT_MODE_12

x0 = abs(MIN_VOLT_MODE_12)
x1 = abs(MAX_VOLT_MODE_12)

count = (x0+x1)/STEP_CALIBRATE

y0 = 0 # начало индекса массива
y1 = count #        конец индекса массива

b = (count*x0)/(x0+x1)
a = b/x0

x = volt # /Ktr!!!!!!!!!!!!!!
y = a*x+b
# y = floor(a*x+b) # искомый индекс в массиве!!! найти минимальное значение от него 26.5-->26 через floor

#     #  TODO:Найти по найденому индексу значение в массиве!  и найти значение n+1

Ca0 = 2592
Ca1 = 2820


a0 = (y * STEP_CALIBRATE) - abs( MIN_VOLT_MODE_12)#TODO: не правльно переводит значения!27*200 = 5400
a1 = ((y+1) * STEP_CALIBRATE) -abs( MIN_VOLT_MODE_12)# TODO: не правльно переводит значения!28*200 = 5600

CA= Ca1-Ca0
Aa = a1-a0
AXXX = x-a0

CodeX = (((CA)/(Aa)))*(AXXX)+Ca0

CodeX = (((Ca1-Ca0)/(a1-a0)))*(x-a0)+Ca0

#--------------------------------------------------------

Ktr = 1.04
minVolt = -9360
maxVolt = 9360

x0 = abs(minVolt)
x1 = abs(maxVolt)

step = 1040
count = (x0+x1)/step


y0 = 0
y1 = count

b = (count*x0)/(x0+x1)
a = b/x0

x = 2400 #/ Ktr!!!!!!!!!!!!!!
y = a*x+b
print(y)
#--------------------------
aX = x

Ca0 = 2592
Ca1 = 2820
a0 = 2080
a1 = 3120

CA= Ca1-Ca0
Aa = a1-a0
AXXX = x-a0

CodeX = (((CA)/(Aa)))*(AXXX)+Ca0
CodeX_ = (((Ca1-Ca0)/(a1-a0)))*(aX-a0)+Ca0
print((CodeX))















# # Параметры щупа
# step_calibrate = 200
# MIN_VOLT_MODE_12 = -12000
# MAX_VOLT_MODE_12 = 12000
# length_mode_12v = int(((-1)*MIN_VOLT_MODE_12+MAX_VOLT_MODE_12)/step_calibrate)

# MIN_VOLT_MODE_27 = -27000
# MAX_VOLT_MODE_27 = 27000
# length_mode_27v = int(((-1)*MIN_VOLT_MODE_27+MAX_VOLT_MODE_27)/step_calibrate)



# dacValA = 0
# dacValB = 0

# dacValZero_m12 = 2048
# dacValMin_A_m12 = 0
# dacValMax_A_m12 = 4095
# dacValMin_B_m12 = 0
# dacValMax_B_m12 = 4095

# dacValZero_m27 = 2048
# dacValMin_A_m27 = 0
# dacValMax_A_m27 = 4095
# dacValMin_B_m27 = 0
# dacValMax_B_m27 = 4095





















