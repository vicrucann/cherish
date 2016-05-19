#ifndef SCALINGGUI_H
#define SCALINGGUI_H


/*! A class that allows to define scaling of the main widget sizes depending on the screen DPI.
*/
class ScalingGui
{
public:
    ScalingGui(int dpiX, int dpiY);

    /*! A method to reset DPI and scaling values, for example, when DPI is changed for some reason.
     * \param dpiX is the DPI for X dimension
     * \param dpiY is the DPI for Y dimension */
    void reset(int dpiX, int dpiY);

protected:
    void setSizes();

private:
    int m_dpiX;
    int m_dpiY;
};

#endif // SCALINGGUI_H
