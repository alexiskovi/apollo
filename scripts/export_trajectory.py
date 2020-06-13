import sys
import os
from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter
from cyber_py.record import RecordReader
import common.proto_utils as proto_utils
import math

from modules.canbus.proto import chassis_pb2
from modules.localization.proto import localization_pb2
from modules.control.proto import control_cmd_pb2

def parse_args():
    parser = ArgumentParser(description='Extracting trajectory from cyber bags', formatter_class=ArgumentDefaultsHelpFormatter)
    parser.add_argument('out_file', help='Filename to write results')
    parser.add_argument('in_bag', help='Path to input bags', nargs='*')
    args = parser.parse_args()
    return args

def write_to_file(args, x, y, v, d, t):
    f_can = open("can" + args.out_file, "w")
    f_trj = open("path" + args.out_file, "w")
    for i in range(len(v)):
        if i % 100 == 0:
            print str(i)+"/"+str(len(v))
        f_can.write(str(v[i]) + " " + str(d[i]) + " " + str(t[i]) + '\n')
    print str(len(v))+"/"+str(len(v))
    for i in range(len(x)):
        if i % 100 ==0:
            print str(i)+"/"+str(len(x))
        f_trj.write(str(x[i]) + " " + str(y[i]) + '\n')
    print str(len(x))+"/"+str(len(x))
    f_can.close()
    f_trj.close()

def main():
    first_can = True
    first_point = True
    x = []
    y = []
    v = []
    d = []
    t = []
    args = parse_args()
    localization = localization_pb2.LocalizationEstimate()
    chassis = chassis_pb2.Chassis()
    control = control_cmd_pb2.ControlCommand()
    for bag_name in args.in_bag:
        reader = RecordReader(bag_name)
        for msg in reader.read_messages():
            if msg.topic == '/apollo/localization/pose':
                localization.ParseFromString(msg.message)
                if first_point:
                    x0 = localization.pose.position.x
                    y0 = localization.pose.position.y
                    first_point = False
                x.append(localization.pose.position.x - x0)
                y.append(localization.pose.position.y - y0)
            if msg.topic == '/apollo/canbus/chassis':
                chassis.ParseFromString(msg.message)
                if first_can:
                    t0 = chassis.header.timestamp_sec
                    first_can = False
                v.append(chassis.speed_mps)
                d.append(chassis.steering_percentage)
                t.append(chassis.header.timestamp_sec - t0)
    
    write_to_file(args, x, y, v, d, t)


if __name__ == '__main__':
    try:
        main()
    	print 'READY'
    except Exception as e:
        raise Exception(e)
