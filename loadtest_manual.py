from distutils import cmd
import os


def manual_spawn_client(count):
  print("Spawn Client " + str(count))
  logName = "customLog/client_" + str(count) + "_log.txt"
  cmd = "./runRoom.sh > " + logName + " fg"
  os.system(cmd)

def main():
  cnt = 0
  while True:
    key = input("Enter n to spawn new client: ")
    if (key == "n" or key == "N"):
      manual_spawn_client(cnt)
      cnt += 1

if __name__ == "__main__":
  main()
