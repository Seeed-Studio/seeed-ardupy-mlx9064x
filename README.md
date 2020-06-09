# seeed-ardupy-MLX9064x [![Build Status](https://travis-ci.com/Seeed-Studio/seeed-ardupy-MLX9064x.svg?branch=master)](https://travis-ci.com/Seeed-Studio/seeed-ardupy-MLX9064x)

## Introduction

The MLX9064x is a fully calibrated 16x12 pixels IR array in an industry standard 4-lead TO39 package with digital interface The MLX9064x contains 192 FIR pixels. An ambient sensor is integrated to measure the ambient temperature of the chip and supply sensor to measure the VDD. The outputs of all sensors IR, Ta and VDD are stored in internal RAM and are accessible through I2C.

You can get more information in here [Grove-Thermal-Imaging](https://www.seeedstudio.com/Grove-Thermal-Imaging-Camera-IR-Array-MLX90641-110-degree-p-4612.html).

## How to binding with ArduPy

- Install [AIP](https://github.com/Seeed-Studio/ardupy-aip)

- Build firmware with Seeed ArduPy MLX9064x

```shell
aip install Seeed-Studio/seeed-ardupy-MLX9064x
aip build
```

- Flash new firmware to you ArduPy board

```shell
aip flash # + Ardupy Bin PATH
```

For more examples of using AIP, please refer to [AIP](https://github.com/Seeed-Studio/ardupy-aip).

## Usage

```python
from arduino import grove_mlx9064x
import time

mlx90641 = grove_mlx9064x()

mlx90641.set_refresh_rate(mlx90641.rate._8HZ)

while True:
    data = mlx90641.get_raw_data()
    print(data)
    time.sleep(1)
```

## API Reference

- **get_frame_data()** :  get frame data (after interpolate image)

```python
data = mlx90641.get_frame_data()
print(data)
```

- **get_frame_data()** :  get raw data 

```python
data = mlx90641.get_raw_data()
print(data)
```

- **set_refresh_rate()** : set refresh rate

```python
mlx90641.set_refresh_rate(mlx90641.rate._8HZ)  #recommend
# mlx90641.set_refresh_rate(mlx90641.rate._0_5HZ)
# mlx90641.set_refresh_rate(mlx90641.rate._1HZ)
# mlx90641.set_refresh_rate(mlx90641.rate._2HZ)
# mlx90641.set_refresh_rate(mlx90641.rate._4HZ)
# mlx90641.set_refresh_rate(mlx90641.rate._16HZ)
# mlx90641.set_refresh_rate(mlx90641.rate._32HZ)
# mlx90641.set_refresh_rate(mlx90641.rate._64HZ)
```

----

This software is written by seeed studio<br>
and is licensed under [The MIT License](http://opensource.org/licenses/mit-license.php). Check License.txt for more information.<br>

Contributing to this software is warmly welcomed. You can do this basically by<br>
[forking](https://help.github.com/articles/fork-a-repo), committing modifications and then [pulling requests](https://help.github.com/articles/using-pull-requests) (follow the links above<br>
for operating guide). Adding change log and your contact into file header is encouraged.<br>
Thanks for your contribution.

Seeed Studio is an open hardware facilitation company based in Shenzhen, China. <br>
Benefiting from local manufacture power and convenient global logistic system, <br>
we integrate resources to serve new era of innovation. Seeed also works with <br>
global distributors and partners to push open hardware movement.<br>
