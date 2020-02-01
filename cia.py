import ciavelli

class CiaPackage(ciavelli.Package):
    def build(self):
        print("Doing secret build right now!")
    def install(self):
        print("Doing secret install right now!")
