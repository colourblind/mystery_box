import os
import sys
import json
import camera_path

def render(camera, template, config_filename, filename):
    camera_pos = '{0} {1} {2}'.format(camera[0][0], camera[0][1], camera[0][2])
    camera_target = '{0} {1} {2}'.format(camera[1][0], camera[1][1], camera[1][2])
    frame_config = template.replace('{camera_pos}', camera_pos)
    frame_config = frame_config.replace('{camera_target}', camera_target)
    frame_config = frame_config.replace('{output_file}', filename)
    f = open(config_filename, 'w')
    f.write(frame_config)
    f.flush()
    f.close()
    execute_line = os.path.join('.', 'ray_march') + ' ' + config_filename + ' > nul'
    os.system(execute_line)
        
def go(c):
    template = config_template
    track = camera_path.Pather(c['key_frames'])
    if not os.path.exists(c['output_dir']):
        os.mkdir(c['output_dir'])
    renderer = c['renderer']
    template = template.replace('{scale}', str(renderer['scale']))
    template = template.replace('{light_pos}', str(renderer['light_pos']))
    template = template.replace('{bailout}', str(renderer['bailout']))
    template = template.replace('{width}', str(renderer['width']))
    template = template.replace('{height}', str(renderer['height']))
    if len(sys.argv) > 2:
        start = int(sys.argv[2])
    else:
        start = 0
    for i in xrange(start, int(c['key_frames'][-1]['time'] * c['fps'])):
        t = float(i) / c['fps']
        camera = track.get(t)
        filename = os.path.join(c['output_dir'], str(i))
        render(camera, template, 'go.cfg', filename)

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
    config = json.load(f)
    f.close()
    go(config)
