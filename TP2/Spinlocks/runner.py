import subprocess

def makefile():
  subprocess.run('g++ -pthread -o adder.out adder.cpp'.split())

def main():
  N = [1e7, 1e8, 1e9]
  K = [1, 2, 4, 8, 16, 32, 64, 128, 256]

  file = open('report2.csv', 'w')
  executable = './adder.out'

  for n in N:
    for k in K:
      command = [executable, str(n), str(k)]
      output = subprocess.run(command, stdout = file)
      # handle(output.returncode)

  file.close()

if __name__ == '__main__':
  makefile()
  main()