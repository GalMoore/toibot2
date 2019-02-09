#!/usr/bin/env python2
from __future__ import print_function

import sys
import rospy
from std_msgs.msg import String
from sensor_msgs.msg import Image

import time

import os
import sys
import numpy as np

from PIL import Image as Im
import numpy as np

try:
    import tensorflow as tf
except ImportError:
    print("unable to import TensorFlow. Is it installed?")
    print("  sudo apt install python-pip")
    print("  sudo pip install tensorflow")
    sys.exit(1)

# ROS related imports
import rospy
from std_msgs.msg import String , Header
from sensor_msgs.msg import Image

# Object detection module imports
import object_detection
from object_detection.utils import label_map_util
from object_detection.utils import visualization_utils as vis_util

# SET FRACTION OF GPU YOU WANT TO USE HERE
GPU_FRACTION = 0.4

######### Set model here ############
MODEL_NAME =  'maytronics_ssd'
# By default models are stored in data/models/
MODEL_PATH = os.path.join(os.path.dirname(sys.path[0]),'data','models' , MODEL_NAME)
# Path to frozen detection graph. This is the actual model that is used for the object detection.
PATH_TO_CKPT = MODEL_PATH + '/frozen_inference_graph_maytronics.pb'  #'/frozen_inference_graph.pb'
######### Set the label map file here ###########
LABEL_NAME = 'labelmap_maytronics.pbtxt'#'mscoco_label_map.pbtxt'
# By default label maps are stored in data/labels/
PATH_TO_LABELS = os.path.join(os.path.dirname(sys.path[0]),'data','labels', LABEL_NAME)
######### Set the number of classes here #########
NUM_CLASSES = 4

detection_graph = tf.Graph()
with detection_graph.as_default():
    od_graph_def = tf.GraphDef()
    with tf.gfile.GFile(PATH_TO_CKPT, 'rb') as fid:
        serialized_graph = fid.read()
        od_graph_def.ParseFromString(serialized_graph)
        tf.import_graph_def(od_graph_def, name='')

## Loading label map

label_map = label_map_util.load_labelmap(PATH_TO_LABELS)
categories = label_map_util.convert_label_map_to_categories(label_map, max_num_classes=NUM_CLASSES, use_display_name=True)
category_index = label_map_util.create_category_index(categories)

config = tf.ConfigProto()
config.gpu_options.per_process_gpu_memory_fraction = GPU_FRACTION

class Detector:

    def __init__(self):

        self.sess = tf.Session(graph=detection_graph,config=config)
        print('init')


    def detect(self, pImg ):

        image = np.array(pImg)

        image_np = np.asarray(pImg)

        image_np_expanded = np.expand_dims(image_np, axis=0)
        image_tensor = detection_graph.get_tensor_by_name('image_tensor:0')
        boxes = detection_graph.get_tensor_by_name('detection_boxes:0')

        scores = detection_graph.get_tensor_by_name('detection_scores:0')
        classes = detection_graph.get_tensor_by_name('detection_classes:0')
        num_detections = detection_graph.get_tensor_by_name('num_detections:0')

        start = time.time()
        (boxes, scores, classes, num_detections) = self.sess.run([boxes, scores, classes, num_detections],
            feed_dict={image_tensor: image_np_expanded})

        end = time.time()
        print('total time for detection ' + str(end - start) )

        objects = vis_util.visualize_boxes_and_labels_on_image_array(
            image,
            np.squeeze(boxes),
            np.squeeze(classes).astype(np.int32),
            np.squeeze(scores),
            category_index,
            use_normalized_coordinates=True,
            line_thickness=2)

        for i in range(len(objects)):
            image_height,image_width,channels = image.shape
            d = (objects[0][2])
            left =  d[1] * image_width
            right = d[3] * image_width
            top =  d[0] *  image_height
            bottom =d[2] * image_height
            w = right - left
            h = bottom - top
            top_left = [left,top]
            object_id=objects[0][0]
            # print(str(w))
            # print(str(h))
            # print(str(top_left))
            # print(str(object_id))

        #print('finish detect')

        # image_out = Im.fromarray(image, 'RGB')
        # image_out.show()
        # image_out.save('/home/yakir/Downloads/out.png')





def main(args):

  obj = Detector()
  infilename = '/home/yakir/Downloads/c.png'
  imgTmp = Im.open(infilename)
  imgTmp.load()


  for x in range(0,30):
    obj.detect(imgTmp)


  print('finish')


  rospy.init_node('image_converter', anonymous=True)
#   try:
#     rospy.spin()
#   except KeyboardInterrupt:
#     print("Shutting down")

if __name__ == '__main__':
    main(sys.argv)
