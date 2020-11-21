# Copyright 2020 James Pace
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import blinkt_interface
import paho.mqtt.client
import json
import time
import argparse

def string_to_color(color, brightness):
    if color == 'red':
        return blinkt_interface.color.red(brightness)
    elif color == 'lime':
        return blinkt_interface.color.lime(brightness)
    elif color == 'green':
        return blinkt_interface.color.green(brightness)
    elif color == 'blue':
        return blinkt_interface.color.blue(brightness)
    elif color == 'white':
        return blinkt_interface.color.white(brightness)
    elif color == 'aqua':
        return blinkt_interface.color.aqua(brightness)
    elif color == 'off':
        return blinkt_interface.color.off(brightness)
    else:
        return None


class Node:
    def __init__(self, broker_addr, rate):
        self._rate = rate

        self._client = paho.mqtt.client.Client(userdata=self,
                                               protocol=paho.mqtt.client.MQTTv5,
                                               transport="tcp")
        self._client.connect(broker_addr)
        self._client.subscribe([("led_state", paho.mqtt.client.SubscribeOptions(qos=1))])
        self._client.message_callback_add("led_state", self._callback)

        self._blinkt = blinkt_interface.Blinkt()

    @staticmethod
    def _callback(client, userdata, message):
        message = json.loads(message.payload)
        color = string_to_color(message['color'], message['brightness'])
        if color:
            userdata._blinkt.setPixel(message['index'], color)
            self._blinkt.display()

    def loop(self):
        self._client.loop_start()

        while True:
            time.sleep(1)


def main():
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('broker', nargs='?', default='localhost', type=str, help='Broker address.')
    args = parser.parse_args()
    node = Node(args.broker)

    node.loop()


if __name__=='__main__':
    main()
