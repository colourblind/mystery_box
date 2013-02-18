import sys
import os
import time
import threading
import json
import anim
import camera_path

THREAD_COUNT = 4

def go(thread_no, dispatcher):
    frame_no, frame_config = dispatcher.get_next_frame()
    while frame_no >= 0:
        print('{0}: Starting frame {1}'.format(thread_no, frame_no))
        anim.render(frame_config, 'go-{0}.cfg'.format(thread_no), hide_output = True)
        frame_no, frame_config = dispatcher.get_next_frame()
    
class Dispatcher(object):
    def __init__(self, config, first_frame = 0):
        self.config = config
        self.track = camera_path.Pather(config['key_frames'])
        self.max_frame = int(config['key_frames'][-1]['time'] * config['fps'])
        self.first_frame = first_frame
        self.next_frame = self.first_frame
        self.lock = threading.Lock()
        
    def get_next_frame(self):
        if self.next_frame >= self.max_frame:
            return -1, None
        else:
            self.lock.acquire()
            frame_no = self.next_frame
            self.next_frame = self.next_frame + 1
            self.lock.release()
            
            frame_config = self.config['renderer']
            t = float(frame_no) / self.config['fps']
            camera = self.track.get(t)
            filename = os.path.join(c['output_dir'], str(frame_no) + '.png')
            frame_config['camera_pos'] = '{0} {1} {2}'.format(camera[0][0], camera[0][1], camera[0][2])
            frame_config['camera_target'] = '{0} {1} {2}'.format(camera[1][0], camera[1][1], camera[1][2])
            frame_config['output_file'] = filename
            
            return frame_no, frame_config

if __name__ == '__main__':
    f = open(sys.argv[1], 'r')
    c = json.load(f)
    f.close()
    
    if not os.path.exists(c['output_dir']):
        os.mkdir(c['output_dir'])
    
    if len(sys.argv) > 2:
        start = int(sys.argv[2])
    else:
        start = 0
    dispatcher = Dispatcher(c, start)
    
    for i in xrange(THREAD_COUNT):
        thread = threading.Thread(name=str(i), target=go, args=(i, dispatcher,))
        thread.daemon = True
        thread.start()
        
    while threading.active_count() > 1:
        time.sleep(5)
  