#!/usr/bin/env python

import rospy
from std_msgs.msg import Int8MultiArray, Int16MultiArray
from sensor_msgs.msg import LaserScan
import numpy as np

ARRAY_SIZE = 11

def avg(lst):
	total = 0
	for i in lst:
		total+=i
	return total/len(lst)

class Obst_Avoid():
	def __init__(self):

		self.obst_array = [-3, -8, -10, -8, -3]
		self.speed_array = [1, 1, 1, 1, 1, 0, -1, -2.4, -4, -6, -10]

		self.dir_array = [0]*ARRAY_SIZE
		self.array = np.zeros([2, ARRAY_SIZE])
		self.caution = 0

		self.msg = Int8MultiArray()

		rospy.init_node('obst_avoid')
		rospy.Subscriber('/olin/laser/scan', LaserScan, self.laser_cb)
		self.obst_pub = rospy.Publisher('/obst/cmd_vel', Int8MultiArray, queue_size=1)

	def laser_cb(self, msg):
		self.dir_array = [0]*ARRAY_SIZE
		self.caution = 0
		scan = msg.ranges
		width = float(len(scan))/(ARRAY_SIZE)
		for direction in range(ARRAY_SIZE)[2:ARRAY_SIZE-3]:
			dist = avg(scan[int(np.floor(direction*width)):int(np.ceil(direction*width+1))])
			caution = (abs(10*(direction - (ARRAY_SIZE-1)/2.)/ARRAY_SIZE)+dist-10)
			self.caution = max(self.caution, caution)
			obst_array = [i*(10-dist**2/10) for i in self.obst_array]
			offset = direction-2
			self.dir_array[offset:offset+5] = [min(obst_array[i], self.dir_array[offset+i]) for i in range(5)]
		for i in range(ARRAY_SIZE):
			if self.dir_array[i]<-100:
				self.dir_array[i] = -100
		if self.caution<-10:
			self.caution=-10
			

	def test(self):
		r = rospy.Rate(10)
		while not rospy.is_shutdown():
			self.array[0] = [-i*self.caution**2/10 for i in self.speed_array]
			self.array[1] = self.dir_array
			print self.array
			#self.array[0] = [i*self.obst**2/10. for i in self.speed_array]
			#self.array[1][5-2:5+3] = [i*self.obst**2/10. for i in self.obst_array]
			self.msg.data = self.array.reshape([1, 2*ARRAY_SIZE]).astype(int).tolist()[0]
			self.obst_pub.publish(self.msg)
			r.sleep()

if __name__ == "__main__":
	test = Obst_Avoid()
	test.test()
