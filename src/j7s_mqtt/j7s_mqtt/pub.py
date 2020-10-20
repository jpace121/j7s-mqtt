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

import paho.mqtt.client
import argparse
import numpy as np
import json
import time

class Node:
    def __init__(self, broker_addr, rate, color, index):
        self._rate = rate
        self._msg = {}
        self._msg['color'] = color
        self._msg['index'] = index
        self._msg['brightness'] = 0

        self._time = 0

        self._client = paho.mqtt.client.Client(userdata=self,
                                               protocol=paho.mqtt.client.MQTTv5,
                                               transport="tcp")
        self._client.connect(broker_addr)
        self._topic = "led_state"

    def loop(self):
        self._client.loop_start()

        while True:
            self._msg['brightness'] = np.sin(self._time)

            payload = json.dumps(self._msg)
            resp = self._client.publish(self._topic, payload)
            resp.wait_for_publish()

            self._time = self._time + 1.0/self._rate
            time.sleep(1.0/self._rate)


def main():
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('broker', nargs='?', default='localhost', type=str, help='Broker address.')
    parser.add_argument('rate', nargs='?', default=10.0, type=float, help='Rate to publish new samples.')
    parser.add_argument('color', nargs='?', default='white', type=str, help='Color for index.')
    parser.add_argument('index', nargs='?', default=0, type=int, help='Index for blinkt.')
    args = parser.parse_args()

    node = Node(args.broker, args.rate, args.color, args.index)

    node.loop()

if __name__ == '__main__':
    main()
