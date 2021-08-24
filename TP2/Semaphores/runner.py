import subprocess

def makefile():
  subprocess.run('g++ -std=c++11 -pthread -o produtorConsumidor.out produtorConsumidor.cpp'.split())

def main():
  N = [1, 2, 4, 16, 32]
  NPNC = [(1,1),(1,2),(1,4),(1,8),(1,16),(2,1),(4,1),(8,1),(16,1)]

  file = open('report.csv', 'w')
  executable = './produtorConsumidor.out'

  for n in N:
    for x in NPNC:
      command = [executable, str(n), str(x[0]), str(x[1])]
      output = subprocess.run(command, stdout = file)
      # handle(output.returncode)

  file.close()

if __name__ == '__main__':
  makefile()
  main()