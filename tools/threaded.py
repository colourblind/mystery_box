import sys
import os
import time
import threading
import json
import anim
import camera_path

THREAD_COUNT = 4

def go(thread_no, c, track, template, dispatcher):
    # Fetch next frame number
    frame = dispatcher.get_next_frame()
    while frame >= 0:
        t = float(frame) / c['fps']
        camera = track.get(t)
        filename = os.path.join(c['output_dir'], str(frame))
        print('{0}: Starting frame {1}'.format(thread_no, frame))
        anim.render(camera, template, 'go-{0}.cfg'.format(thread_no), filename)
        frame = dispatcher.get_next_frame()
    
class Dispatcher(object):
    def __init__(self, max_frame, first_frame = 0):
        self.max_frame = max_frame
        self.first_frame = first_frame
        self.next_frame = self.first_frame
        
    def get_next_frame(self):
        if self.next_frame >= self.max_frame:
            return -1
        else:
            self.next_frame = self.next_frame + 1
            return self.next_frame - 1

        
config_template = """scale: {scale}
camera_pos: {camera_pos}
camera_target: {camera_target}
light_pos: {light_pos}
output_file: {output_file}.png
width: {width}
height: {height}
bailout: {bailout}
fov: 90
"""

if __name__ == '__main__':
    f = open(sys.argv[1], 'r')
    c = json.load(f)
    f.close()
    
    track = camera_path.Pather(c['key_frames'])
    execute_line = os.path.join('.', 'ray_march') + ' go.cfg'
    if not os.path.exists(c['output_dir']):
        os.mkdir(c['output_dir'])
    renderer = c['renderer']
    template = config_template
    template = template.replace('{scale}', str(renderer['scale']))
    template = template.replace('{light_pos}', str(renderer['light_pos']))
    template = template.replace('{bailout}', str(renderer['bailout']))
    template = template.replace('{width}', str(renderer['width']))
    template = template.replace('{height}', str(renderer['height']))
    
    if len(sys.argv) > 2:
        start = int(sys.argv[2])
    else:
        start = 0
    dispatcher = Dispatcher(int(c['key_frames'][-1]['time'] * c['fps']), start)
    
    for i in xrange(THREAD_COUNT):
        thread = threading.Thread(name=str(i), target=go, args=(i, c, track, template, dispatcher,))
        thread.daemon = True
        thread.start()
        
    while True:
        time.sleep(100)
  