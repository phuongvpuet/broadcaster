import math
import sys
import os


def main():
  params = sys.argv
  numberClients = 1
  delaySpawn = 0
  startRoomName = ""
  if (len(params) >= 2):
    startRoomName = params[1]
  if (len(params) >= 3):
    numberClients = params[2]
  if (len(params) >= 4):
    delaySpawn = params[3]
  if (len(startRoomName) < 3):
    print("Syntax Error, please use: python3 loadtest.py ROOM_NAME NUMBER_CLIENT");
    return
  print("Number Clients: " + str(numberClients))
  print("DelayTime Spawn: " + str(delaySpawn))
  print("Start Room Name: " + startRoomName)
  command = ""
  serverUrl = "https://portal-mediasoup-dev.service.zingplay.com:4443"
  print("----------Start Load Test-----------")
  for i in range(int(numberClients)):
    logName = "customLog/client_" + str(i) + "_log.txt"
    roomId = startRoomName + "_" + str(math.floor(i / 2))
    bash = "SERVER_URL=" + serverUrl + " " + "ROOM_ID=" + roomId + " build/broadcaster"
    command += "sleep " + str(int(delaySpawn) + i) + " && " + bash + " > " + logName + " fg"
    print("Client " + str(i) + " join room " + roomId)
    if (i < int(numberClients) - 1):
      command += " & "
  # print("Command: " + command)
  os.system(command)
if __name__ == "__main__":
	main()
