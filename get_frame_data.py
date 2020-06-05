from arduino import grove_mlx9064x
import time

mlx90641 = grove_mlx9064x()

while True:
    # data = mlx90641.get_frame_data()
    # print(data)
    mlx90641.get_frame_data()
    time.sleep(1)
