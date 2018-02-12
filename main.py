import subprocess
import sys
from generateImg import generateImage


def main():
	if len(sys.argv) < 3:
		print("Error : ")
		print(" - first argument : instance size")
		print(" - second argument : number of instance wanted")
		exit(-1)

	instance_size = int(sys.argv[1])
	number_instance = int(sys.argv[2])

	generateInstances(instance_size, number_instance)
	generateFootprint(instance_size, number_instance)

	print("All footprints pictures are generated in folder footprints/")


def generateInstances(instance_size, number):
	bash_command = "QAPGenerators/doInstances.sh " + str(instance_size) + " " + str(number)
	process = subprocess.Popen(bash_command.split(), stdout=subprocess.PIPE)
	process.wait()


def generateFootprint(instance_size, number):
	binary = "random_walk/build/randomWalk"
	seed = "1"
	walk_length = "500"

	for i in range(number):
		instance_file_rl = "instances/qap_rl_" + str(instance_size) + "_" + str(i) + ".dat"
		output_file_rl = "random_walk/data/footprint_rl_" + str(instance_size) + "_" + str(i) + ".dat"
		bash_command = str(binary) + " " + str(instance_file_rl) + " " + str(seed) + " " + str(walk_length) + " " + str(output_file_rl)
		process = subprocess.Popen(bash_command.split(), stdout=subprocess.PIPE)
		process.wait()

		instance_file_uni = "instances/qap_uni_" + str(instance_size) + "_" + str(i) + ".dat"
		output_file_uni = "random_walk/data/footprint_uni_" + str(instance_size) + "_" + str(i) + ".dat"
		bash_command_uni = str(binary) + " " + str(instance_file_uni) + " " + str(seed) + " " + str(walk_length) + " " + str(output_file_uni)
		process_uni = subprocess.Popen(bash_command_uni.split(), stdout=subprocess.PIPE)
		process_uni.wait()

		generateImage(output_file_uni, "footprints/fp_uni_" + str(instance_size) + "_" + str(i) + ".png")
		generateImage(output_file_rl, "footprints/fp_rl_" + str(instance_size) + "_" + str(i) + ".png")


if __name__ == '__main__':
	main()
