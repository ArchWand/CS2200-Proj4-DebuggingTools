# CS 2200 Project 4 Local Autograder by ArcWand
# v1.1

OUTPUTDIR='output'
STATSFILE='stats.txt'
BIN='os-sim'
TRIALS=15

# Comment out the tests you don't want to run
TEST={
        #  (alg, params, cores), context switches, execution time, wait time

        ('fcfs', None, 1): ((101.95, 15.72), (70.19, 11.59), (168.54, 13.89)),
        ('fcfs', None, 2): ((131.50, 18.61), (43.13, 14.18), (30.52, 17.55)),
        ('fcfs', None, 4): ((176.29, 12.52), (37.01, 10.69), (0.19, 10.59)),

        ('pa', 0, 1): ((164.99, 12.28), (77.80, 11.37), (275.91, 12.40)),
        ('pa', 0, 2): ((163.43, 20.97), (45.71, 12.84), (98.35, 27.02)),
        ('pa', 0, 4): ((177.73, 11.48), (37.00, 10.11), (0.15, 10.75)),
        ('pa', 1, 1): ((94.00, 10.00), (67.90, 10.11), (366.69, 13.88)),
        ('pa', 1, 2): ((133.25, 24.31), (39.51, 11.57), (53.61, 10.72)),
        ('pa', 1, 4): ((176.73, 12.52), (37.01, 10.35), (0.13, 10.36)),
        ('pa', 2, 1): ((94.00, 11.14), (67.91, 11.83), (372.15, 24.72)),
        ('pa', 2, 2): ((125.60, 12.92), (39.83, 10.15), (48.24, 19.09)),
        ('pa', 2, 4): ((176.72, 12.46), (37.00, 10.34), (0.13, 10.35)),
        ('pa', 3, 1): ((94.00, 10.00), (67.90, 10.00), (372.10, 10.00)),
        ('pa', 3, 2): ((125.67, 13.86), (39.83, 10.15), (48.09, 18.78)),
        ('pa', 3, 4): ((176.80, 12.49), (37.01, 10.72), (0.13, 10.37)),

        ('rr', 2, 1): ((362.97, 11.20), (67.91, 10.25), (283.90, 17.20)),
        ('rr', 2, 2): ((397.67, 21.40), (40.46, 11.21), (41.82, 19.19)),
        ('rr', 2, 4): ((445.61, 18.47), (37.20, 11.29), (0.63, 12.43)),
        ('rr', 4, 1): ((201.99, 11.14), (67.92, 10.25), (291.93, 17.45)),
        ('rr', 4, 2): ((231.85, 25.07), (40.78, 11.52), (42.35, 23.43)),
        ('rr', 4, 4): ((284.91, 15.92), (37.41, 11.29), (0.46, 12.93)),
        ('rr', 8, 1): ((130.95, 11.33), (67.93, 10.36), (317.85, 16.94)),
        ('rr', 8, 2): ((162.77, 25.20), (40.46, 11.25), (45.16, 21.36)),
        ('rr', 8, 4): ((213.66, 14.82), (36.97, 11.35), (0.76, 13.00))

    }

def main():
    generate_stats()
    stats = calculate_stats()
    print_stats(stats, STATSFILE)
    print_stats(stats, None) # print to stdout

import sys
import os

def get_dir(alg, param, ncores):
    return f'{OUTPUTDIR}/{alg.lower()}/{ncores}_core/{get_param_name(alg, param)}'

def get_output_file(alg, param, ncores, trial):
    return f'{get_dir(alg, param, ncores)}/trial_{trial:0{len(str(TRIALS-1))}d}.txt'

def alg2flag(alg):
    match alg:
        case 'fcfs': return None
        case 'pa': return '-p'
        case 'rr': return '-r'
        case _: return None

def get_param_name(alg, param):
    match alg:
        case 'fcfs': return '-'
        case 'pa': return f'weight-{param}'
        case 'rr': return f'time-{param}00ms'
        case _: return '-'

def get_test_name(test):
    alg = test[0].upper()
    param = get_param_name(test[0], test[1])
    ncores = test[2]
    return f'{alg} {param}, {ncores} core'

def cmd_run_bin(alg, param, ncores):
    cmd = f'./{BIN} {ncores}'
    if flag := alg2flag(alg):
        cmd += f' {flag} {param}'
    return cmd

def cmd_tee(file):
    return f'tee {file}'

def cmd_parse_stats():
    #  pattern = '^Total .*: ([[:digit:]]*\\.?[[:digit:]]?)( s)?'
    #  replace = '\\1'
    #  cmd = f"tail -3 | sed -E 's/{pattern}/{replace}/'"
    #  return cmd
    return 'tail -3'

def cmd_tag(s):
    return f'cat <(echo "{s}") -'

def test(alg, param, ncores, trial):
    dir = get_dir(alg, param, ncores)
    file = get_output_file(alg, param, ncores, trial)
    if not os.path.exists(dir): os.makedirs(dir)

    cmd = cmd_run_bin(alg, param, ncores)
    cmd += ' > ' + file
    #  cmd += ' | ' + cmd_tee(file)
    #  cmd += ' | ' + cmd_parse_stats()
    #  cmd += ' | ' + cmd_tag(cmd_run_bin(alg, param, ncores));
    #  cmd += ' >> ' + STATSFILE
    os.system(cmd)

def generate_stats():
    if os.path.exists(OUTPUTDIR):
        if os.listdir(OUTPUTDIR):
            if input(f'{OUTPUTDIR} is not empty, clear? [y/n] ').lower() == 'n':
                return
            else:
                os.system(f'rm -r {OUTPUTDIR}')
        else:
            os.rmdir(OUTPUTDIR)

    for alg, param, ncores in TEST.keys():
        for trial in range(TRIALS):
            print(f'\r{get_test_name((alg, param, ncores))}: {trial+1}/{TRIALS}',
                  '          ', end='')
            test(alg, param, ncores, trial)
    print()

def mean(data):
    return sum(data) / len(data)

def var(data):
    l = min(data)
    m = mean(data)
    h = max(data)
    return max(m - l, h - m)
    #  return (sum((x - m) ** 2 for x in data) / len(data)) ** 0.5

def calculate_stats():
    # Read the data from the files
    data = {}
    for t in TEST.keys():
        data[t] = {'context switches': [], 'execution time': [], 'wait time': []}
        for trial in range(TRIALS):
            file = get_output_file(t[0], t[1], t[2], trial)
            print(f'\rReading {file}          ', end='')
            # store only the last 3 lines
            with open(file) as f:
                lines = f.readlines()[-3:]
                context_switches = int(lines[0].split(':')[1].strip())
                execution_time = float(lines[1].rsplit(' ', 2)[1])
                wait_time = float(lines[2].rsplit(' ', 2)[1])
                data[t]['context switches'].append(context_switches)
                data[t]['execution time'].append(execution_time)
                data[t]['wait time'].append(wait_time)

    stats = {
        t: {
            'context switches': (mean(data[t]['context switches']), var(data[t]['context switches'])),
            'execution time': (mean(data[t]['execution time']), var(data[t]['execution time'])),
            'wait time': (mean(data[t]['wait time']), var(data[t]['wait time']))
        }
        for t in TEST.keys()
    }
    print()
    return stats

def out_of_bound(mean, target, margin):
    return mean < target - margin or mean > target + margin

def error_format(stat_line, target, margin, color=False):
    stat_line = '*' + stat_line[1:-1] + f' ({target} ± {margin})'
    if color:
        stat_line = f'\033[91m{stat_line}\033[0m' # ]]
    return stat_line + '\n'

def print_stats(stats, file):
    with open(file, 'w') if file else sys.stdout as f:
        prev_alg, prev_param, prev_cores = None, -1, -1
        for t in TEST.keys():
            alg, param, ncores = t
            if alg != prev_alg:
                f.write('='*40 + '\n')
                f.write('    ' + ((alg.upper()+"    ") * 3) + '\n')
            if param != prev_param:
                f.write('-'*40 + '\n')
                if param:
                    f.write(f'{get_param_name(alg, param)}\n')
            if ncores != prev_cores:
                f.write(f'{ncores} core' + ('\n' if ncores == 1 else 's\n'))

            cs_mean, cs_stddev = stats[t]['context switches']
            et_mean, et_stddev = stats[t]['execution time']
            wt_mean, wt_stddev = stats[t]['wait time']

            cs_info = f'  context switches: {cs_mean:>6.2f} ± {cs_stddev:>4.2f}\n'
            et_info = f'  execution time:   {et_mean:>6.2f} ± {et_stddev:>4.2f}\n'
            wt_info = f'  wait time:        {wt_mean:>6.2f} ± {wt_stddev:>4.2f}\n'

            if out_of_bound(cs_mean, *TEST[t][0]):
                cs_info = error_format(cs_info, *TEST[t][0], file is None)
            if out_of_bound(et_mean, *TEST[t][1]):
                et_info = error_format(et_info, *TEST[t][1], file is None)
            if out_of_bound(wt_mean, *TEST[t][2]):
                wt_info = error_format(wt_info, *TEST[t][2], file is None)

            f.write(cs_info)
            f.write(et_info)
            f.write(wt_info)
            f.write('\n')

            prev_alg, prev_param, prev_cores = alg, param, ncores

        f.write('-'*40 + '\n')
        f.write('='*40 + '\n')

if __name__ == '__main__':
    main()
