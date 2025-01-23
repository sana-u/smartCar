# -*- coding: utf-8 -*-
import sys
import os

import cv2
import time

import numpy as np



try:
    import onnxruntime as ort
except ModuleNotFoundError as e:
    print(f"Error importing onnxruntime: {e}")
    raise

class yolov5_lite():
    def __init__(self, model_pb_path, label_path, confThreshold=0.5, nmsThreshold=0.5):
        so = ort.SessionOptions()
        so.log_severity_level = 3
        self.net = ort.InferenceSession(model_pb_path, so)
        self.classes = list(map(lambda x: x.strip(), open(label_path, 'r').readlines()))

        self.confThreshold = confThreshold
        self.nmsThreshold = nmsThreshold
        self.input_shape = (self.net.get_inputs()[0].shape[2], self.net.get_inputs()[0].shape[3])

    def letterBox(self, srcimg, keep_ratio=True):
        top, left, newh, neww = 0, 0, self.input_shape[0], self.input_shape[1]
        if keep_ratio and srcimg.shape[0] != srcimg.shape[1]:
            hw_scale = srcimg.shape[0] / srcimg.shape[1]
            if hw_scale > 1:
                newh, neww = self.input_shape[0], int(self.input_shape[1] / hw_scale)
                img = cv2.resize(srcimg, (neww, newh), interpolation=cv2.INTER_AREA)
                left = int((self.input_shape[1] - neww) * 0.5)
                img = cv2.copyMakeBorder(img, 0, 0, left, self.input_shape[1] - neww - left, cv2.BORDER_CONSTANT,
                                         value=0)  # add border
            else:
                newh, neww = int(self.input_shape[0] * hw_scale), self.input_shape[1]
                img = cv2.resize(srcimg, (neww, newh), interpolation=cv2.INTER_AREA)
                top = int((self.input_shape[0] - newh) * 0.5)
                img = cv2.copyMakeBorder(img, top, self.input_shape[0] - newh - top, 0, 0, cv2.BORDER_CONSTANT, value=0)
        else:
            img = cv2.resize(srcimg, self.input_shape, interpolation=cv2.INTER_AREA)
        return img, newh, neww, top, left

    def postprocess(self, frame, outs, pad_hw):
        newh, neww, padh, padw = pad_hw
        frameHeight = frame.shape[0]
        frameWidth = frame.shape[1]
        ratioh, ratiow = frameHeight / newh, frameWidth / neww
        classIds = []
        confidences = []
        boxes = []
        for detection in outs:
            scores, classId = detection[4], detection[5]
            if scores > self.confThreshold:  # and detection[4] > self.objThreshold:
                x1 = int((detection[0] - padw) * ratiow)
                y1 = int((detection[1] - padh) * ratioh)
                x2 = int((detection[2] - padw) * ratiow)
                y2 = int((detection[3] - padh) * ratioh)
                classIds.append(classId)
                confidences.append(scores)
                boxes.append([x1, y1, x2, y2])

        # # Perform non maximum suppression to eliminate redundant overlapping boxes with
        # # lower confidences.
        
        #indices = cv2.dnn.NMSBoxes(boxes, confidences, self.confThreshold, self.nmsThreshold)
        indices = self.NMSBoxes(boxes, confidences, self.nmsThreshold)
        
        for ind in indices :
            frame = self.drawPred(frame, classIds[ind], confidences[ind], boxes[ind][0], boxes[ind][1], boxes[ind][2], boxes[ind][3])
        return frame

    def drawPred(self, frame, classId, conf, x1, y1, x2, y2):
        # Draw a bounding box.
        cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 0, 255), thickness=2)

        label = '%.2f' % conf
        text = '%s:%s' % (self.classes[int(classId)], label)
        #print("classId:",classId)

        # Display the label at the top of the bounding box
        labelSize, baseLine = cv2.getTextSize(text, cv2.FONT_HERSHEY_SIMPLEX, 0.5, 1)
        y1 = max(y1, labelSize[1])
        cv2.putText(frame, text, (x1, y1 - 10), cv2.FONT_HERSHEY_TRIPLEX, 0.5, (0, 255, 0), thickness=1)
        return frame

    def detect(self, srcimg):
        img, newh, neww, top, left = self.letterBox(srcimg)
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        img = img.astype(np.float32) / 255.0
        blob = np.expand_dims(np.transpose(img, (2, 0, 1)), axis=0)

        t1 = time.time()
        outs = self.net.run(None, {self.net.get_inputs()[0].name: blob})[0]
        cost_time = time.time() - t1
        #print("type:",type(outs))
        
        if outs is not None and len(outs) > 0:
          print(outs[0][5])
        else:
          print(-1)

        #srcimg = self.postprocess(srcimg, outs, (newh, neww, top, left))
        #infer_time = 'Inference Time: ' + str(int(cost_time * 1000)) + 'ms'
        #cv2.putText(srcimg, infer_time, (5, 20), cv2.FONT_HERSHEY_TRIPLEX, 0.5, (0, 0, 0), thickness=1)
        return outs#srcimg
        
    def NMSBoxes(self,boxes, scores, threshold):
      if len(boxes) == 0:
          return []
      boxes = np.array(boxes)
  
      # ����߽�����
      areas = (boxes[:, 2] - boxes[:, 0]) * (boxes[:, 3] - boxes[:, 1])
      
      scores = np.array(scores)
  
      # �����Ŷȵ÷ֽ�������߽��
      order = scores.argsort()[::-1]
  
      keep = []
      while order.size > 0:
          i = order[0]
          keep.append(i)
  
          # ���㽻����������
          xx1 = np.maximum(boxes[i, 0], boxes[order[1:], 0])
          yy1 = np.maximum(boxes[i, 1], boxes[order[1:], 1])
          xx2 = np.minimum(boxes[i, 2], boxes[order[1:], 2])
          yy2 = np.minimum(boxes[i, 3], boxes[order[1:], 3])
  
          # ���㽻������Ŀ��Ⱥ͸߶�
          w = np.maximum(0.0, xx2 - xx1)
          h = np.maximum(0.0, yy2 - yy1)
  
          # ���㽻����������
          inter = w * h
  
          # ���㽻���ȣ�IoU��
          iou = inter / (areas[i] + areas[order[1:]] - inter)
  
          # ���� IoU С����ֵ�ı߽��
          inds = np.where(iou <= threshold)[0]
          order = order[inds + 1]
  
      return keep


def yolo_main():
    # ����Ĭ�ϲ���
    modelpath = 'LR.onnx'
    classfile = 'LR.names'
    confThreshold = 0.4
    nmsThreshold = 0.6

    '''
    parser = argparse.ArgumentParser()
    parser.add_argument('--modelpath', type=str, default='best.onnx', help="onnx filepath")
    parser.add_argument('--classfile', type=str, default='coco.names', help="classname filepath")
    parser.add_argument('--confThreshold', default=0.2, type=float, help='class confidence')
    parser.add_argument('--nmsThreshold', default=0.6, type=float, help='nms iou thresh')
    args = parser.parse_args()
    '''
    
    net = yolov5_lite(modelpath, classfile, confThreshold, nmsThreshold=nmsThreshold)

    # ������ͷ
    cap = cv2.VideoCapture(0)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

   
   # ������ͷ��ȡһ֡ͼ��
    ret, frame = cap.read()
    if frame is None:
      print("�޷���ȡͼ��")
      return None

    outs = net.detect(frame.copy())

    
    #return outs
yolo_main()
  

    