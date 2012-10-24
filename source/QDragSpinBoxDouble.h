#ifndef _DRAG_SPIN_BOX_DOUBLE_H_
#define _FLOAT_SPIN_BOX_H_

#include <QDoubleSpinBox>
/*
class QDragSpinBoxDouble : public QDoubleSpinBox
{
  Q_OBJECT
public:
  enum { MUNG_OFF, MUNG_VERTICAL, MUNG_HORIZONTAL };

public:
  FloatSpinBox ( QWidget * parent = 0, const char * name = 0 ) ;
  FloatSpinBox ( double minValue, double maxValue, double step = 0.1, 
                 QWidget * parent = 0, const char * name = 0 ) ;
  ~FloatSpinBox();

  /** @reimp */
  void stepUp()   { QSpinBox::stepUp(); }
  /** @reimp */
  void stepDown() { QSpinBox::stepDown(); }
  void stepUp(int howMany);
  void stepDown(int howMany);

  void setCursorWarping(bool onOff);
  bool cursorWarping() const;

  void setDoubleFormat(char f='g', int prec = 4);

  // Theese are for Designer too, but could be useful otherwise as well.
  void setDoubleMinValue ( const QString& min ) ;
  void setDoubleMaxValue ( const QString& max ) ;
  void setDoubleLineStep ( const QString& step ) ;
  void setDoubleValue    ( const QString& val ) ;
  QString doubleMinValueStr ( ) const ;
  QString doubleMaxValueStr ( ) const ;
  QString doubleLineStepStr ( ) const ;
  QString doubleValueStr ( ) const ;

  // These are the ones you should use normally.
  double doubleMinValue ( ) const ;
  double doubleMaxValue ( ) const ;
  double doubleLineStep ( ) const ;
  double doubleValue ( )    const ;

public slots:
  void setDoubleRange    ( double min, double max ) ;
  void setDoubleMinValue ( double min ) ;
  void setDoubleMaxValue ( double max ) ;
  void setDoubleLineStep ( double step ) ;
  void setDoubleValue    ( double val ) ;

private:
  // Overrides to hide methods that shouldn't be called by users
  // because they won't have the expected behavior.
  // Really I'd like to inherit privately from QSpinBox, but
  // still inherit from it's parent publicly.  Not possible in C++, though.
  /** @reimp */
  int value() const    { return QSpinBox::value(); }
  /** @reimp */
  int minValue() const { return QSpinBox::minValue(); }
  /** @reimp */
  int maxValue() const { return QSpinBox::maxValue(); }
  /** @reimp */
  int lineStep() const { return QSpinBox::lineStep(); }
  /** @reimp */
  void setMinValue(int v) { QSpinBox::setMinValue(v); }
  /** @reimp */
  void setMaxValue(int v) { QSpinBox::setMaxValue(v); }
  /** @reimp */
  void setLineStep(int v) { QSpinBox::setLineStep(v); }

private slots:
  /** @reimp */
  void setValue(int v)  { QSpinBox::setValue(v); }


signals:
  void valueChanged ( double );

protected:
  int     mapTextToValue( bool* );
  QString mapValueToText( int v );
  void    valueChange( ) ;

  void mousePressEvent(QMouseEvent*);
  void mouseReleaseEvent(QMouseEvent*);
  void mouseMoveEvent(QMouseEvent*);
  void keyPressEvent(QKeyEvent *);

  bool eventFilter(QObject*, QEvent*);

private:
  void _init( );
  double m_increment;
  int m_mungMode;
  bool m_warpCursor;
  int m_moveLeftover;
  QPoint m_lastPos;
  char m_floatFormat;
  int m_floatPrec;
};
*/

#endif
