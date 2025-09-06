def gerar_etiqueta():
    return """SIZE 22 mm,10 mm
GAP 2 mm,0
DENSITY 8
SPEED 3
DIRECTION 0
CLS
TEXT 10,10,"1",0,1,1,"L.:001/25"
TEXT 10,30,"1",0,1,1,"FAB.:23/JAN/2025"
TEXT 10,50,"1",0,1,1,"VAL.:23/JAN/2027"
PRINT 1
"""

def imprimir_etiqueta(comando):
    try:
        with open("/dev/usb/lp0", "wb") as impressora:
            impressora.write(comando.encode("utf-8"))
        print("✅ Etiqueta enviada com sucesso!")
    except Exception as e:
        print(f"❌ Erro ao imprimir: {e}")

if __name__ == "__main__":
    comando = gerar_etiqueta()
    imprimir_etiqueta(comando)
