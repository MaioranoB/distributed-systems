import subprocess
from shutil import copy2

def makefile():
  subprocess.run('g++ -lpthread -o client.out client.cpp'.split())
  subprocess.run('g++ -lpthread -o coordinator.out coordinator.cpp'.split())

scalability_test_list = [
  {'r': '10', 'k': '2', 'n': [2]},
  {'r': '10', 'k': '1', 'n': [2,4,8,16,32]}, #
  {'r':  '5', 'k': '1', 'n': [2,4,8,16,32,64]},
  {'r':  '3', 'k': '0', 'n': [2,4,8,16,32,64,128]}
]

def main():
  src = 'resultado.txt'
  client_executable = './client.out'
  coordinator_executable = './coordinator.out'
  # subprocess.run(coordinator_executable)

  for test_index, test_setting in enumerate(scalability_test_list):
    for n in test_setting['n']:
      command = [client_executable, str(n), test_setting['r'], test_setting['k']]
      print(command)
      subprocess.run(command)
      dst = f'./teste{test_index}/resultadoN={n}.txt'
      copy2(src, dst)

if __name__ == '__main__':
  makefile()
  main()