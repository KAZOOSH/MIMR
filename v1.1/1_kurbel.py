import instrument

print "starting kurbel"

kurbel = Instrument(14,1)

# update instrument
while True:
	kurbel.update()