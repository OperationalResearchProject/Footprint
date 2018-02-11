import QAPGenerators.makeQAPrl
import subprocess
import generateImg

def main():
	for i in range(0,5):
		bashCommand = "random_walk/build/randomWalk QAPGenerators/instance.dat 1 500 footprint.dat"
		process = subprocess.Popen(bashCommand.split(), stdout=subprocess.PIPE)
		output, error = process.communicate()
		generateImg.main("footprint.dat",str(i+6)+".png")

if __name__ == '__main__':
    main()
