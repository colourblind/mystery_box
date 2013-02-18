import os
import sys
import json
import camera_path
import subprocess

def render(config, config_filename, hide_output = False):
    frame_config = json_to_config(config)
    f = open(config_filename, 'w')
    f.write(frame_config)
    f.close()
    if hide_output:
        out = open(os.devnull, 'w')
    else:
        out = None
    subprocess.call([os.path.join('.', 'ray_march'), config_filename], stdout=out)
        
def go(c):
    track = camera_path.Pather(c['key_frames'])
    if not os.path.exists(c['output_dir']):
        os.mkdir(c['output_dir'])

    renderer = c['renderer']
    
    if len(sys.argv) > 2:
        start = int(sys.argv[2])
    else:
        start = 0
        
    for i in xrange(start, int(c['key_frames'][-1]['time'] * c['fps'])):
        t = float(i) / c['fps']
        camera = track.get(t)
        filename = os.path.join(c['output_dir'], str(i) + '.png')
        renderer['camera_pos'] = '{0} {1} {2}'.format(camera[0][0], camera[0][1], camera[0][2])
        renderer['camera_target'] = '{0} {1} {2}'.format(camera[1][0], camera[1][1], camera[1][2])
        renderer['output_file'] = filename
        render(renderer, 'go.cfg')
        
def json_to_config(data):
    return '\n'.join(['{0}: {1}'.format(key, data[key]) for key in data])        

if __name__ == '__main__':
    f = open(sys.argv[1], 'r')
    config = json.load(f)
    f.close()
    go(config)
