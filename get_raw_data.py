from arduino import grove_mlx9064x
import time

mlx90641 = grove_mlx9064x()

mlx90641.set_refresh_rate(mlx90641.rate._8HZ)

while True:
    data = mlx90641.get_raw_data()
    print(data)
    time.sleep(1)
