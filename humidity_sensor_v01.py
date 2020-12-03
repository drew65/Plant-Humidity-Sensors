#!/usr/bin/python3
#%matplotlib notebook
import socket
import json
import time
import os
import shutil
from pathlib import Path
from datetime import datetime, timedelta
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.animation as animation
import matplotlib.ticker as ticker
#import chart_studio.plotly as py
#import plotly.graph_objects as go

UDP_IP = "192.168.0.19"
UDP_PORT = 2390
UDP_REMOTE_IP = "192.168.0.50"

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))


class DataCollect() :
    def __init__(self, name, deltaFormat) :
        self.size = len(deltaFormat)
        print("size {}".format(len(deltaFormat)))
        self.startTime = []
        self.count = []
        self.deltaTime = []
        self.data = []
        self.deltaFormat = []
        self.yl = []
        self.ylabel = []
        self.name = name
        self.xrange = []
        for i in range(len(deltaFormat)):
            if deltaFormat[i][1] == 'days':
                self.deltaTime.append(24 * 60  * deltaFormat[i][0])
            elif deltaFormat[i][1] == 'hours':
                self.deltaTime.append(60  * deltaFormat[i][0])
            elif deltaFormat[i][1] == 'minutes':
                self.deltaTime.append(deltaFormat[i][0])
            self.deltaFormat.append(deltaFormat[i][2])
            self.count.append(0)
            self.startTime.append(0)
            self.ylabel.append(deltaFormat[i][3])
            self.xrange.append(deltaFormat[i][4])
            print(self.count)
            coord = []
            coord.append([])
            coord.append([])
            self.data.append(coord)
    
    def addData(self, y):
        dtNow = datetime.now()
        self.yl.append(y)
        print("{}  yl = {}".format(self.name, self.yl[-1]))
        for i in range(self.size):
            if self.count[i] == 0:
                self.startTime[i] = dtNow
            elif dtNow >= self.startTime[i] + timedelta(minutes=self.deltaTime[i]):
                self.data[i][0] = self.data[i][0][-self.xrange[i]:]
                self.data[i][1] = self.data[i][1][-self.xrange[i]:]
                self.data[i][0].append(dtNow.strftime(self.deltaFormat[i]))
                if i == 0:
                    self.data[i][1].append(sum(self.yl[-self.count[i]:])/self.count[i])
                else:
                    self.data[i][1].append(sum(self.data[i-1][1][-self.count[i]:])/self.count[i])
                print(self.data[i][0][-1])
                print("y{} = {}".format(i, self.data[i][1][-1]))
                print("count{} : {}".format(i, self.count[i]))
                self.count[i] = 0
                self.startTime[i] = dtNow
                if i < self.size-1:
                    self.count[i+1] += 1
        self.count[0] += 1
    
    

class Plotter():
    def __init__(self, data):
        self.rows = len(data["plant1"].data)
        self.cols = len(data)
        self.fig, self.axs = plt.subplots(self.rows, self.cols)
       

    def plot2(self, data):
        #size = len(data)
        print("len data : {}".format(len(data)))
        j = 0
        for plant in data.keys():
            print("len data[{}]: {}   struct : {}".format(data[plant].name, len(data[plant].data), data[plant].data))
            size = data[plant].size
            if size > self.rows:
                print("check size {}  {}".format(size, self.rows))
                #data.data = data.data[:-size+self.number]
                size = self.rows
            for i in range(size):
                self.axs[i][j].clear()
                print("range {}".format(size-i-1))
                self.axs[i][j].plot(data[plant].data[(size-i-1)][0], data[plant].data[(size-i-1)][1])
            j += 1
        #self.fig.suptitle(data.name)
        #self.fig.tight_layout()

    def plot(self, data):
        #size = len(data)
        print("len data : {}".format(len(data)))
        j = 0
        for plant in data.keys():
            print("len data[{}]: {}   struct : {}".format(data[plant].name, len(data[plant].data), data[plant].data))
            size = data[plant].size
            if size > self.rows:
                print("check size {}  {}".format(size, self.rows))
                #data.data = data.data[:-size+self.number]
                size = self.rows
            for i in range(size):
                self.axs[i][j].clear()
                #print("range {}".format(size-i-1))
                self.axs[i][j].plot(data[plant].data[(size-i-1)][0], data[plant].data[(size-i-1)][1])
            for i in range(size-1): 
                plt.sca(self.axs[i][j])
                plt.xticks(rotation=45, ha='right')
                plt.subplots_adjust(hspace = 0.40)
                if j==0:
                    self.axs[i][j].set(ylabel=data[plant].ylabel[i])
                    plt.yticks(rotation=0)
                if i==0:
                    self.axs[i][j].set_title(data[plant].name)
            plt.sca(self.axs[-1][j])
            plt.xticks(rotation=45, ha='right')
            plt.subplots_adjust(bottom=0.10)
            if j==0:
                self.axs[-1][j].set(ylabel=data[plant].ylabel[-1])
                plt.yticks(rotation=0)
            j += 1
        #self.fig.suptitle(data.name)
        self.fig.tight_layout()
              
    
class Sensor():
    def __init__(self, configfile=''):
        self.config = None
        theconfigfile = Path(configfile)
        if not theconfigfile.is_file(): 
            theconfigfile = Path("{}_bck".format(configfile))
            if theconfigfile.is_file():
                shutil.move("{}_bck".format(configfile),configfile)
            else:
                print('Failed to find config file.')
                raise Exception('Failed to find config file.')

        try:
            self.config = json.load(open(configfile,'r'))
        except json.decoder.JSONDecodeError:
            theconfigfile = Path("{}_bck".format(configfile))
            if theconfigfile.is_file():
                shutil.move("{}_bck".format(configfile),configfile)
            else:
                try:
                    self.config = json.load(open("{}_bck".format(configfile),'r'))
                except json.decoder.JSONDecodeError:
                    print('Failed to find config file.')
                    raise Exception('Failed to load config file.')

                shutil.move("{}_bck".format(configfile),configfile)
                self.config = json.load(open("{}_bck".format(configfile),'r'))
        except:
            print('Failed to find config file.')
            raise Exception('Failed to load config file.')
        print("Sensor config loaded")
        self.confMessage = {}
        self.json_confMessage = {}
        self.nbrOfItems = int(self.config["devices"][0]["nbrItems"])
        print("nbr of items : {}".format(self.nbrOfItems))
        for i in range(self.config["nbrDevices"]):
            self.confMessage[self.config["devices"][i]["ipAddress"]], self.json_confMessage[self.config["devices"][i]["ipAddress"]] = self.buildConfigMessage(self.config["devices"][i])
        self.data = {}
        self.plots = {}
        test2 = {}
        for j in range(self.config["nbrDevices"]):
            for i in range(self.config["devices"][j]["nbrItems"]):
                test2[self.config["devices"][j]["items"][i]["name"]] = self.config["devices"][j]["items"][i]["plots"]
        print("test22 : {}".format(test2))
        for plantName  in list(test2.keys()):
            self.data[plantName] = DataCollect(plantName, test2[plantName])
        self.plot = Plotter(self.data)
        print("DATA  ")
        print(self.data)
        self.ani = animation.FuncAnimation(self.plot.fig, self.animate2, interval=1000)
        plt.show()

    def buildConfigMessage(self, json_plot):
        message = {}
        message["message"] = "transmit"
        message["type"] = "config"
        message["ipAddress"] = json_plot["ipAddress"]
        message["port"] = json_plot["port"]
        message["nbrItems"] = json_plot["nbrItems"]
        items = []
        for i in range(message["nbrItems"]):
            item = {}
            item["type"] = json_plot["items"][i]["type"]
            item["subType"] = json_plot["items"][i]["subType"]
            item["name"] = json_plot["items"][i]["name"]
            item["power"] = json_plot["items"][i]["power"]
            item["data"] = json_plot["items"][i]["data"]
            item["mode"] = json_plot["items"][i]["mode"]
            items.append(item)
        message["items"] = items
        print("self.confMessage : {}".format(message))
        json_message = json.dumps(message)
        return message, json_message

    def animate2(self, i):
        print(datetime.now().strftime('%H:%M:%S.%f'))
        download, ipaddr = sock.recvfrom(2056) # buffer size is 1024 bytes
        print("remot ip {} : {}  {}".format(ipaddr, ipaddr[0], ipaddr[1]))
        text = download.decode('utf-8')
        try:
            json_obj = json.loads(text)
            print(text)
        except:
            print("Something went wrong : {}".format(text))
        else:
            if json_obj["message"] == "request":
                if json_obj["type"] == "config":
                    if ipaddr[0] in self.confMessage.keys():
                        sock.sendto(self.json_confMessage[ipaddr[0]].encode(), (ipaddr[0], ipaddr[1]))
                    time.sleep(5)
                    print("send config")
            elif json_obj["message"] == "transmit" and json_obj["type"] == "data":
                # Add x and y to lists
                for i in range(json_obj["nbrItems"]):
                    name = json_obj["item"][i]["name"]
                    if name in self.data.keys():
                        self.data[name].addData(json_obj["item"][i]["reading2"])
                self.plot.plot(self.data)
        


if __name__ == "__main__":
    from sys import argv
    #import syslog
    import os
    print(argv)
    try:
        script, aconfigfile = argv
    except ValueError:
        print('SensorApp failed to start wrong number of arguments.')
        exit(1)
    print("Good 2 go")
    sensors = Sensor(configfile=aconfigfile)
    print("test")
    