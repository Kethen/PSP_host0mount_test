set -xe

pspsh -e 'mkdir ms0:/PSP/GAME/host0_test' | true
pspsh -e 'cp EBOOT.PBP ms0:/PSP/GAME/host0_test/'
pspsh -e 'cp PARAM.SFO ms0:/PSP/GAME/host0_test/'
