#! /usr/bin/env python

from __future__ import division
import sys
import numpy as np
import cv2
import json

ball_height = 17.5
fov = np.deg2rad( 41.41 ) / 480
fov = np.deg2rad( 53.5 ) / 640

try:
    with open("prefs.json") as f:
        prefs = json.load(f)
except:
    prefs = {}
lo = np.array(prefs.get("lo", [0,0,0]))
hi = np.array(prefs.get("hi", [180,255,255]))

cv2.namedWindow('main')
cv2.createTrackbar("Hl", "main",lo[0],180,lambda v: lo.__setitem__(0,v))
cv2.createTrackbar("Hh", "main",hi[0],180,lambda v: hi.__setitem__(0,v))
cv2.createTrackbar("Sl", "main",lo[1],255,lambda v: lo.__setitem__(1,v))
cv2.createTrackbar("Sh", "main",hi[1],255,lambda v: hi.__setitem__(1,v))
cv2.createTrackbar("Vl", "main",lo[2],255,lambda v: lo.__setitem__(2,v))
cv2.createTrackbar("Vh", "main",hi[2],255,lambda v: hi.__setitem__(2,v))

cap = cv2.VideoCapture(1)
#cap.set(3,64000); #setting to a ridiculous size to get max HxW
#cap.set(4,48000);

def color_mask(bgr):
    hsv = cv2.cvtColor(bgr, cv2.COLOR_BGR2HSV)
    mask = cv2.inRange(hsv, lo, hi)
    mask = cv2.erode(mask, None, iterations=2)
    mask = cv2.dilate(mask, None, iterations=2)
    return mask

def find_targets(mask):
    targets = []
    for c in cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)[-2]:
    	((x, y), r) = cv2.minEnclosingCircle(c)
    	if r > 20:
            a = fov * 2 * r
            d = ball_height / np.sin(a)
            #d = ball_height / np.tan(a)
            targets.append({"x":int(x),"y":int(y),"r":int(r),"d":d,"A":np.rad2deg(a),"f":d*0.0328084})
    return targets

while True:
    r, frame = cap.read()
    mask = color_mask(frame)
    frame = cv2.bitwise_and(frame, frame, mask=mask)
    for t in find_targets(mask):
       cv2.circle(frame, (t["x"], t["y"]), t["r"], (0, 255, 255), 2)
       cv2.circle(frame, (t["x"], t["y"]), 20, (180, 180, 255), 2)
       tl = (t["x"] - t["r"], t["y"] - t["r"])
       txt = "f:{f:.1f} d:{d:.1f} r:{r:.1f} A:{A:.2f}".format(**t)
       cv2.putText(frame, txt, tl, cv2.QT_FONT_NORMAL, 1, (255, 255, 255), 2, cv2.LINE_AA)
    # small = cv2.resize(img, (0, 0), fx=.5, fy=.5)
    cv2.imshow('main', frame)
    #cv2.imwrite('img.jpg', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()

prefs = {
    "lo": lo.tolist(),
    "hi": hi.tolist()
}
with open("prefs.json", "w") as f:
    json.dump(prefs, f)
