#include "../headers/main_window.h"

#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  // w.show();
  // w.showMaximized();
  w.showFullScreen();
  return a.exec();
}

/*
- Persistencia: guardar los dibujos como png/jpg o guardar el estado del pizarrón
- Pintar puntos
- Usar QSettings: para guardar el ultimo color y grosor utilizado o el estado de la ventana
    QSettings settings("MiEmpresa", "MiPizarra");
    settings.setValue("lastColor", m_currentColor.name());
    // Al iniciar:
    m_currentColor = QColor(settings.value("lastColor", "#000000").toString());
- Dibujar cada de selección estética a los trazos, cambiar CustomPathItem para que dibuje su propio recuadro de selección estilizado sobreescribiendo el método paint().
*/


// find . -type f \( -name "*.h" -o -name "*.cpp" -o -name "*.pro" \) -not -path "*/build/*" -exec sh -c 'echo "=== ARCHIVO: {} ==="; cat "{}"; echo "\n"' \; > "code_$(date +%Y_%m_%d_%H_%M).txt"