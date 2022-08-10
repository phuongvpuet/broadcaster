import sys
import os


def main():
  params = sys.argv
  numberClients = 1
  delaySpawn = 0
  if (len(params) >= 2):
    numberClients = params[1]
  if (len(params) >= 3):
    delaySpawn = params[2]
  print("Number Clients: " + str(numberClients))
  print("DelayTime Spawn: " + str(delaySpawn))
  command = ""
  for i in range(int(numberClients)):
    print("Spawn Client " + str(i))
    logName = "customLog/client_" + str(i) + "_log.txt"
    command += "sleep " + str(int(delaySpawn) + i) + " && ./runRoom.sh > " + logName + " fg"
    if (i < int(numberClients) - 1):
      command += " & "
  print("Command: " + command)
  os.system(command)
if __name__ == "__main__":
	main()
