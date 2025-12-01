import pandas as pd
import matplotlib.pyplot as plt
import glob
import os

def graf(archivo_csv):
    print(f"--- Procesando {archivo_csv} ---")
    
    try:
        df = pd.read_csv(archivo_csv)
    except Exception as e:
        print(f"No se pudo leer {archivo_csv}: {e}")
        return

    nombre_base = archivo_csv.replace("resultados_", "").replace(".csv", "")
    
    epsilons = df['Epsilon'].unique()
    epsilons.sort()
    
    df_rank = df[df['TipoConsulta'] == 'RANK']
    
    if not df_rank.empty:
        fig, axes = plt.subplots(1, len(epsilons), figsize=(15, 6), sharey=False)
        if len(epsilons) == 1: axes = [axes]
        
        fig.suptitle(f'Error en Consulta de Rank - {nombre_base}', fontsize=16)
        
        for i, eps in enumerate(epsilons):
            data = df_rank[df_rank['Epsilon'] == eps]
            ax = axes[i]
            
            ax.plot(data['ValorConsulta'], data['ErrorAbsoluto'], 'b-o', label='Error Real', markersize=4)
            
            
            limite = data['ErrorMaxPermitido'].mean()
            ax.axhline(y=limite, color='r', linestyle='--', label=f'Límite Teórico ($\epsilon N$ = {int(limite)})')
            
            ax.set_title(f'$\epsilon$ = {eps}')
            ax.set_xlabel('Valor Consultado (x)')
            ax.set_ylabel('Error Absoluto de Rank')
            ax.legend()
            ax.grid(True, alpha=0.3)
            
            max_err = data['ErrorAbsoluto'].max()
            print(f"  [RANK] Epsilon {eps}: Max Error = {max_err} (Límite = {limite}) -> {'CUMPLE' if max_err <= limite else 'FALLA'}")

        plt.tight_layout()
        nombre_img = f"grafico_rank_{nombre_base}.png"
        plt.savefig(nombre_img)
        print(f"  -> Gráfico guardado: {nombre_img}")
        plt.close()

    df_quant = df[df['TipoConsulta'] == 'QUANTILE']
    
    if not df_quant.empty:
        fig, axes = plt.subplots(1, len(epsilons), figsize=(15, 6), sharey=False)
        if len(epsilons) == 1: axes = [axes]
        
        fig.suptitle(f'Error en Consulta de Quantile - {nombre_base}', fontsize=16)
        
        for i, eps in enumerate(epsilons):
            data = df_quant[df_quant['Epsilon'] == eps]
            ax = axes[i]
            
            ax.bar(data['ValorConsulta'], data['ErrorAbsoluto'], width=0.04, color='orange', alpha=0.7, label='Diferencia de Valor')
            
            ax.set_title(f'$\epsilon$ = {eps}')
            ax.set_xlabel('Quantile Consultado ($\phi$)')
            ax.set_ylabel('Error Absoluto (|Est - Real|)')
            ax.legend()
            ax.grid(True, alpha=0.3)

        plt.tight_layout()
        nombre_img = f"grafico_quantile_{nombre_base}.png"
        plt.savefig(nombre_img)
        print(f"  -> Gráfico guardado: {nombre_img}")
        plt.close()
    print("\n")

def main():
    archivos = glob.glob("resultados_*.csv")
    
    if not archivos:
        print("No se encontraron archivos 'resultados_*.csv'. Ejecuta primero el programa en C++.")
        return

    print(f"Se encontraron {len(archivos)} archivos para procesar.")
    for archivo in archivos:
        graf(archivo)
    
    print("Proceso terminado. Revisa las imágenes PNG generadas.")

if __name__ == "__main__":
    main()