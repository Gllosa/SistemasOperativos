'''
Alberto Garcia Fernandez

Sistemas Operativos - Practica 1 - Llamadas al sistema

Este programa verifica que el formato del entregable de la practica es el correcto (sigue las especificaciones de nombrado, y esta bien comprimido).

Tambien realiza una prueba basica de funcionamiento para cada uno de los tres programas solicitados en la practica. Cumplir esta prueba no es garantia de tener la maxima nota en el ejercicio. Se trata solo de una sugerencia para que los alumnos comprueben el funcionamiento general de su programa. Los alumnos deberan ademas cumplir los otros requisitos del programa, realizar el codigo adecuado, comentarlo, probar casos extremos, y en general cumplir con las demas exigencias descritas en el enunciado de la practica. El programa crea un entorno de prueba en la carpeta /tmp/ssoo/ y crea los siguientes objetos:

Directorio dirC
Directorio dirA
Fichero f_empty.txt (vacio)
Fichero	f_aes.txt (contiene "AAAAA")
Copia de los ficheros fuente del alumno.
Ejecutables compilados a partir de los ficheros fuente del alumno.

Con todos estos objetos, realiza pruebas basicas del mycat, myls y mysize
'''
import subprocess
import signal
import os
import glob
import time
import sys

resultString=""
result=0

def normalizeOutput(result):
	'''
	Funcion que normaliza una cadena de entrada con el fin de eliminar errores de formato. Reemplaza saltos de linea por tabuladores y elimina espacios repetidos y espacios al principio y final
	'''
	result = result.replace("\r\n","\n");
	result = result.replace("\r","\n");
	resultList = result.split("\n")
	resultNormalized = ""
	for resultLine in resultList:
		if resultLine == "":
			continue
		resultLine = resultLine.replace("\t", " ")
		resultLine = resultLine.replace("  ", " ")
		resultLine = resultLine.strip()
		resultNormalized += (resultLine + "\t")

	resultNormalized = resultNormalized.strip()

	return resultNormalized


def lookForTargetFile(folder,targetFile):
	'''
	Funcion que busca un fichero objetivo en una carpeta y devuelve su ruta
	'''
	toReturn="-"
	found=False
	subfolders=[x[0] for x in os.walk(folder)]
	for innerFolder in subfolders:
		folders=os.listdir(innerFolder)
		for item in folders:
			if item.lower()==targetFile.lower():
				return innerFolder+"/"+item
	return toReturn
	

def checkTargetFile(folder,targetFile):
	'''
	Funcion que comprueba que un fichero objetivo existe. Si no existe, lo intenta buscar en las carpetas hijas.
	'''
	toReturn="-"
	filePath=folder+targetFile
	if not os.path.exists(filePath):
		toReturn=lookForTargetFile(folder,targetFile)
	else:
		toReturn=filePath
	return toReturn


def testExerciseCAT(folder):
	'''
	Funcion que realiza la prueba sugerida del programa mycat
	'''

	global resultString 
	global result
	global minipunto
	#Definimos los nombres del fichero de codigo fuente, programa y fichero de prueba
	targetFile="mycat.c"
	programFile="mycat"
	
	#Buscamos el fichero fuente
	filePath=checkTargetFile(folder,targetFile)
	if filePath=="-":
		print "CORRECTOR:", targetFile, "no encontrado"
		return "-"
	
	#Compilamos el fichero fuente
	programPath=folder+programFile
	compiled=subprocess.call(["gcc", filePath,"-o", programPath])
	if compiled != 0:
		print "CORRECTOR:", "error al compilar", targetFile
		return "-"

	#Realizamos la prueba sugerida (cat <fichero>)
	result1 = subprocess.check_output(["cat", folder+testFile])

	#Ejecutamos el programa del alumno
	cmd = programPath + " " + folder+testFile
	pro = subprocess.Popen(cmd, stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True, preexec_fn=os.setsid)  
	time.sleep(0.5)
	os.killpg(pro.pid, signal.SIGTERM) 

	result2, stderr = pro.communicate()

	#Obtenemos las salidas de la prueba con la que vamos a comparar y de la ejecucion del programa del alumno
	expectedResult = normalizeOutput(result1)
	obtainedResult = normalizeOutput(result2)

	#Comparamos las salidas e imprimimos resultados
	print "CORRECTOR MYCAT. Salida esperada:" 
	print result1
	print "CORRECTOR MYCAT. Salida del programa:" 
	print result2

	if result1 == result2:
		print "CORRECTOR MYCAT. PRUEBA CORRECTA"
		resultString += "1 " 
		result += 1.11112
	else:
		print "CORRECTOR MYCAT. PRUEBA INCORRECTA. Las salidas difieren"
		resultString += "0 "
	#Realizamos la prueba sugerida (cat <fichero_big>)
        result1 = subprocess.check_output(["cat", folder+"f_big.txt"])

        #Ejecutamos el programa del alumno
        cmd = programPath + " " + folder+"f_big.txt"
        pro = subprocess.Popen(cmd, stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True, preexec_fn=os.setsid)
        time.sleep(0.5)
        os.killpg(pro.pid, signal.SIGTERM)

        result2, stderr = pro.communicate()

        #Obtenemos las salidas de la prueba con la que vamos a comparar y de la ejecucion del programa del alumno
        expectedResult = normalizeOutput(result1)
        obtainedResult = normalizeOutput(result2)

        #Comparamos las salidas e imprimimos resultados
        print "CORRECTOR MYCAT. Salida esperada:"
        print result1
        print "CORRECTOR MYCAT. Salida del programa:"
        print result2

        if result1 == result2:
                print "CORRECTOR MYCAT. PRUEBA CORRECTA"
                resultString += "1 "
		result += 1.11112
        else:
                print "CORRECTOR MYCAT. PRUEBA INCORRECTA. Las salidas difieren"
                resultString += "0 "
	#Realizamos la prueba sugerida (cat <fichero que no existe>)
        result1 = 255

        #Ejecutamos el programa del alumno
        cmd = programPath + " " + folder+"noexiste.txt"
        pro = subprocess.Popen(cmd, stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True, preexec_fn=os.setsid)
        time.sleep(0.5)
        os.killpg(pro.pid, signal.SIGTERM)

        result2, stderr = pro.communicate()
	result2 = pro.returncode

        #Comparamos las salidas e imprimimos resultados
        print "CORRECTOR MYCAT. Salida esperada:"
        print result1
        print "CORRECTOR MYCAT. Salida del programa:"
        print result2

        if result1 == result2:
                print "CORRECTOR MYCAT. PRUEBA CORRECTA"
		resultString += "1 "
		result += 1.11112
        else:
                print "CORRECTOR MYCAT. PRUEBA INCORRECTA. Las salidas difieren"
		resultString += "0 "
	#Realizamos la prueba sugerida (cat nofile)
        result1 = 255

        #Ejecutamos el programa del alumno
        cmd = programPath 
        pro = subprocess.Popen(cmd, stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True, preexec_fn=os.setsid)
        time.sleep(0.5)
        os.killpg(pro.pid, signal.SIGTERM)

        result2, stderr = pro.communicate()
        result2 = pro.returncode

        #Comparamos las salidas e imprimimos resultados
        print "CORRECTOR MYCAT. Salida esperada:"
        print result1
        print "CORRECTOR MYCAT. Salida del programa:"
        print result2

        if result1 == result2:
                print "CORRECTOR MYCAT. PRUEBA CORRECTA"
		resultString += "1 "
		result += 1.11112
        else:
                print "CORRECTOR MYCAT. PRUEBA INCORRECTA. Las salidas difieren"
		resultString += "0 "
def testExerciseLS(folder):
	'''
	Funcion que realiza la prueba sugerida del programa myls
	'''
	global resultString
	global result
	global minipunto
	#Definimos los nombres del fichero de codigo fuente y programa
	targetFile="myls.c"
	programFile="myls"

	#Buscamos el fichero fuente
	filePath=checkTargetFile(folder,targetFile)
	if filePath=="-":
		print "CORRECTOR:", targetFile, "no encontrado"
		return "-"

	#Compilamos el fichero fuente
	programPath=folder+programFile
	compiled=subprocess.call(["gcc", filePath,"-o", programPath])
	if compiled != 0:
		print "CORRECTOR:", "error al compilar", targetFile
		return "-"

	#Realizamos la prueba sugerida (ls -f1 <carpeta>)
	result1 = subprocess.check_output(["ls","-f1",folder])

	#Ejecutamos el programa del alumno
	cmd = programPath + " " + folder
	pro = subprocess.Popen(cmd, stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True, preexec_fn=os.setsid)  
	time.sleep(0.5)
	os.killpg(pro.pid, signal.SIGTERM) 
	result2, stderr = pro.communicate()

	#Obtenemos las salidas de la prueba con la que vamos a comparar y de la ejecucion del programa del alumno
	expectedResult = normalizeOutput(result1)
	obtainedResult = normalizeOutput(result2)

	#Comparamos las salidas e imprimimos resultados
	print "CORRECTOR MYLS. Salida esperada:" 
	print result1
	print "CORRECTOR MYLS. Salida del programa:" 
	print result2

	if result1 == result2:
		print "CORRECTOR MYLS. PRUEBA CORRECTA"
		resultString += "1 "
		result += 1.11112
	else:
		print "CORRECTOR MYLS. PRUEBA INCORRECTA. Las salidas difieren"
		resultString += "0 "


	#Realizamos la prueba sugerida (ls -f -1 nothing)
        result1 = subprocess.check_output(["ls","-f1"])

        #Ejecutamos el programa del alumno
        cmd = programPath
        pro = subprocess.Popen(cmd, stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True, preexec_fn=os.setsid)
        time.sleep(0.5)
        os.killpg(pro.pid, signal.SIGTERM)
        result2, stderr = pro.communicate()

        #Obtenemos las salidas de la prueba con la que vamos a comparar y de la ejecucion del programa del alumno
        expectedResult = normalizeOutput(result1)
        obtainedResult = normalizeOutput(result2)

        #Comparamos las salidas e imprimimos resultados
        print "CORRECTOR MYLS. Salida esperada:"
        print result1
        print "CORRECTOR MYLS. Salida del programa:"
        print result2

        if result1 == result2:
                print "CORRECTOR MYLS. PRUEBA CORRECTA"
		resultString += "1 "
		result += 1.11112
        else:
                print "CORRECTOR MYLS. PRUEBA INCORRECTA. Las salidas difieren holaaa"
		resultString += "0 "

	#Realizamos la prueba sugerida (ls -f -1 nothing)
        result1 = 255

        #Ejecutamos el programa del alumno
        cmd = programPath+" noexistedirectorio"
        pro = subprocess.Popen(cmd, stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True, preexec_fn=os.setsid)
        time.sleep(0.5)
        os.killpg(pro.pid, signal.SIGTERM)
        result2, stderr = pro.communicate()
	result2 = pro.returncode
        #Obtenemos las salidas de la prueba con la que vamos a comparar y de la ejecucion del programa del alumno

        #Comparamos las salidas e imprimimos resultados
        print "CORRECTOR MYLS. Salida esperada:"
        print result1
        print "CORRECTOR MYLS. Salida del programa:"
        print result2

        if result1 == result2:
                print "CORRECTOR MYLS. PRUEBA CORRECTA"
		resultString += "1 "
		result += 1.11112
        else:
                print "CORRECTOR MYLS. PRUEBA INCORRECTA. Las salidas difieren"
		resultString += "0 "

def testExerciseSIZE(folder):
	'''
	Funcion que realiza la prueba sugerida del programa mysize
	'''
	global resultString
	global result
	global minipunto
	#Definimos los nombres del fichero de codigo fuente y programa
	targetFile="mysize.c"
	programFile="mysize"

	#Buscamos el fichero fuente
	filePath=checkTargetFile(folder,targetFile)
	if filePath=="-":
		print "CORRECTOR:", targetFile, "no encontrado"
		return "-"

	#Compilamos el fichero fuente
	programPath=folder+programFile
	compiled=subprocess.call(["gcc", filePath,"-o", programPath])
	if compiled != 0:
		print "CORRECTOR:", "error al compilar", targetFile
		return "-"

	cwd=os.getcwd()
	os.chdir(folder)

	#Realizfolder+testFileamos la prueba sugerida
	result1 = subprocess.check_output("ls -lU1 | awk \'$2 == 1 {print $9\"\\t\"$5}\'", shell=True)

	#Ejecutamos el programa del alumno
	cmd = programPath
	pro = subprocess.Popen(cmd, stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True, preexec_fn=os.setsid)  
	time.sleep(0.5)
	os.killpg(pro.pid, signal.SIGTERM) 
	result2, stderr = pro.communicate()

	os.chdir(cwd)

	#Obtenemos las salidas de la prueba con la que vamos a comparar y de la ejecucion del programa del alumno
	expectedResult = normalizeOutput(result1)
	obtainedResult = normalizeOutput(result2)

	#Comparamos las salidas e imprimimos resultados
	print "CORRECTOR MYSIZE. Salida esperada:" 
	print result1
	print "CORRECTOR MYSIZE. Salida del programa:" 
	print result2

	if result1 == result2:
		print "CORRECTOR MYSIZE. PRUEBA CORRECTA"
		resultString += "1 "
		result += 1.11112
	else:
		print "CORRECTOR MYSIZE. PRUEBA INCORRECTA. Las salidas difieren"
		resultString += "0 "
	os.chdir(folder+"dirA")
	#Realizfolder+testFileamos la prueba sugerida (mysize con directorio anterior)
        result1 = subprocess.check_output("ls -lU1 | awk \'$2 == 1 {print $9\"\\t\"$5}\'", shell=True)
        #Ejecutamos el programa del alumno
        cmd = "../"+programFile
        pro = subprocess.Popen(cmd, stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True, preexec_fn=os.setsid)
        time.sleep(0.5)
        os.killpg(pro.pid, signal.SIGTERM)
        result2, stderr = pro.communicate()

        os.chdir(cwd)

        #Obtenemos las salidas de la prueba con la que vamos a comparar y de la ejecucion del programa del alumno
        expectedResult = normalizeOutput(result1)
        obtainedResult = normalizeOutput(result2)

        #Comparamos las salidas e imprimimos resultados
        print "CORRECTOR MYSIZE. Salida esperada:"
        print result1
        print "CORRECTOR MYSIZE. Salida del programa:"
        print result2

        if result1 == result2:
                print "CORRECTOR MYSIZE. PRUEBA CORRECTA"
		resultString += "1 "
		result += 1.11112
        else:
                print "CORRECTOR MYSIZE. PRUEBA INCORRECTA. Las salidas difieren"
		resultString += "0 "


if(__name__=="__main__"):
	'''
	Funcion main de la aplicacion. Obtiene el fichero pasado como argumento. Lo descomprime, chequea su formato y finalmente lanza las pruebas.
	'''

	#Comprobamos que se ha pasado un fichero como argumento
	if not len(sys.argv) == 2:
		print 'Uso: python corrector.py <fichero de codigo>'
	else:
		print 'CORRECTOR: corrigiendo', sys.argv[1]
		inputFile = sys.argv[1]
		
		#Comprobamos que el fichero existe
		if not os.path.isfile(inputFile):
			print "El fichero", inputFile, "no existe"
			sys.exit(0)
	
		#Comprobamos el formato del nombre del fichero
		tokens=inputFile.replace(".zip","")
		tokens=tokens.split("_")
		if len(tokens) != 3 and len(tokens) != 4 and len(tokens) != 5:
			print "Formato del nombre del archivo incorrecto: ssoo_p1_AAAAA_BBBBB_CCCCC.zip"
			sys.exit(0)
			
		ssoo=tokens[0]
		p1=tokens[1]
		u1=tokens[2]
		u2=""
		u3=""
		if len(tokens)>3:
			u2=tokens[3]
			if len(tokens)>4:
				u3=tokens[4]
		if not (ssoo == "ssoo" and p1 == "p1"):
			print "Formato del nombre del archivo incorrecto: ssoo_p1_AAAAA_BBBBB_CCCCC.zip"
			sys.exit(0)

		print "CORRECTOR: NIA 1",u1, "NIA 2", u2, "NIA 3", u3
		
		#Preparamos la carpeta temporal donde se realizaran las pruebas
		tempFolder="/tmp/ssoo/"
		testFile="f_aes.txt"
		if os.path.exists(tempFolder):
			subprocess.call(["rm", "-r",tempFolder])
		os.mkdir(tempFolder)
		os.mkdir(tempFolder+"dirC")
		os.mkdir(tempFolder+"dirA")
		subprocess.call(["touch",tempFolder+"f_empty.txt"])		
		subprocess.call("echo \"AAAAA\" > " + tempFolder + testFile, shell=True)
		
		subprocess.call(["cp",inputFile,tempFolder])
		subprocess.call(["touch",tempFolder+"dirA/"+"f_sizetrue.txt"])
                subprocess.call("echo \"AAAAA\" > " + tempFolder + testFile, shell=True)
		#Prepare a file of more than 1024 bytes for the mycat tests
		string_val = 'B' *6000
		subprocess.call(["touch",tempFolder+"f_big.txt"])
                subprocess.call("echo \" "+ string_val+"\" > " + tempFolder + "f_big.txt", shell=True)
		print 'CORRECTOR: descomprimiendo'

		#Descomprimimos el fichero en la carpeta temporal
		zipresult=subprocess.call(["unzip",tempFolder+"/*.zip","-d",tempFolder])
		if not zipresult == 0:
			print "Error al descomprimir el fichero zip"
			sys.exit(0)
		
		#Comprobamos que el fichero de autores existe
		if not os.path.isfile(tempFolder+"Autores.txt"):
			print "El fichero Autores.txt no existe"
			sys.exit(0)

		
		#Realizamos una prueba basica de cada uno de los programas

		testExerciseCAT(tempFolder)

		testExerciseLS(tempFolder)

		testExerciseSIZE(tempFolder)

		resultString
		print(resultString) 
		result
		print "Nota: ", result
		subprocess.call("echo " + str(result) + "> nota.txt", shell=True)










