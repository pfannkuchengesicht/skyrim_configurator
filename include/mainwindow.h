/*
    Skyrim Configurator
    Copyright (C) 2017  Roy Freytag

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QListWidget>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>
#include "backups.h"
#include "saves_profname.h"
#include "cskyrimconfig.h"
#include "cpreviewparser.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0, bool log = false);
    ~MainWindow();
    void setLogging(bool log);

public slots:
    void reloadConfig();
    void PreviewHandlerInt(int v);
    void PreviewHandlerBool(bool v);
    void PreviewHandlerDblH(double v);
    void PreviewHandlerDblL(double v);
    void configImport();
    void configExport();
    void configDoImport(QString filename);
    void openAbout();

private slots:

	void spnGamma_change();
	void ckbFPRT_change();
    void ckbScreenFuScr_change();
	void ckbVSync_change();
	void txtScreenresH_change();
	void txtScreenresW_change();
	void spnFOV_change();
	void ckbFXAA_change();
	void ckbTransAA();
	void sldAA_change();
	void cmbRadBlurLev_change();
	void ckbDOF_change();
	void spnHUDOpacity_change();
	void ckbCompass_change();
	void ckbQuestMarkers_change();
	void ckbQuestMarkersFloat_change();
	void ckbSubtitles_change();
	void ckbSubtitlesDialogue_change();
	void spnMaxParticles_change();
	void ckbThreadedParticles_change();
	void cmbTexQual_change();
	void sldTexFilter_change();
	void ckbThreadedTex_change();
	void spnMaxDecalsModelsPA_change();
	void spnMaxDecalsModels_change();
	void spnMaxDecals_change();
	void spnDecalDrawDistance_change();
	void ckbDrawDecals_change();
	void ckbDrawDecalsmodels_change();
	void ckbThreadedDecals_change();
	void ckbBorderRegion_change();
	void ckbThreadedLOD_change();
	void ckbThreadedMesh_change();
	void ckbThreadedMorpher_change();
	void ckbUnloadOnFT_change();
	void spnItmDist_change();
	void spnObjDist_change();
	void spnActDist_change();
	void spnModelDrawDistance_change();
	void spnGrids_change();
	void spnNearDistance_change();
	void spnSplitDistanceMul_change();
	void spnSkyCellFadeDist_change();
	void spnBlockDistance_change();
	void spnAniTrees_change();
	void spnTreeLoadDistance_change();
	void ckbSkinnedTrees_change();
	void ckbTreeThreads_change();
	void spnTreeDrawDistance_change();
	void spnGrassDensi_change();
	void spnGrassDrawDistance_change();
	void ckbEnhancedGrass_change();
	void spnShadowSplitC_change();
	void cmbShadowMode_change();
	void spnShadowBlur_change();
	void cmbShadowRes_change();
	void spnShadowBias_change();
	void cmbShadowFilter_change();
	void ckbDrawShadows_change();
	void ckbPlrSelfShad_change();
	void ckbTreeShadows_change();
	void ckbGrassShadows_change();
	void ckbLandShadows_change();
	void ckbTorchShadows_change();
	void spnShadowDistExt_change();
	void spnShadowDistInt_change();
	void spnShadowMaskQuarter_change();
	void spnShadowDistFade_change();
	void spnSunUpdate_change();
	void spnSpecFadeDist_change();
	void spnLightFadeDist_change();
	void spnSunThres_change();
	void ckbSimpleLight_change();
	void ckbSunBeams_change();
	void sldWaterMS_change();
	void ckbDepth_change();
	void ckbDisplace_change();
	void ckbRefract_change();
	void ckbWaterLOD_change();
	void ckbWaterShader_change();
	void spnReflectBlur_change();
	void cmbWaterRes_change();
	void ckbReflectExplosions_change();
	void ckbReflectSky_change();
	void ckbSilRefl_change();
	void ckbReflectLand_change();
	void ckbReflectObjects_change();
	void ckbReflectTrees_change();
	void ckbForceHD_change();
	void ckbReflectBlur_change();
    void grpReflect_change();
	void ckbMouseInvertY_change();
	void spnMouseSCalY_change();
	void spnMouseSCalX_change();
	void spnMouseSens_change();
	void ckbMouseAccel_change();
	void ckbAlwaysActive_change();
	void ckbIntro_change();
	void ckbScreenshot_change();
	void ckbFileSelection_change();
	void ckbSavePause_change();
	void ckbSaveWait_change();
	void ckbSaveRest_change();
	void ckbSaveTravel_change();
	void spnAutoSaveInt_change();
    void grpAutoSave_change();
    void ckbBckgrndLoadLip();
    void ckbBckgrndLoadFaceGen();
	

    void on_cmbScreenRes_currentIndexChanged(int index);

    void on_ckbUseCustom_toggled(bool checked);

    void on_sldAA_valueChanged(int value);

    void on_ckbShowPreview_toggled(bool checked);

    void on_sldGamma_valueChanged(int value);

    void on_btnApply_clicked();

    void on_cmbAspect_currentIndexChanged(int index);

    void on_sldTexFilter_valueChanged(int value);

    void on_spnGamma_valueChanged(double arg1);

    void on_sldDecalDrawDistance_valueChanged(int value);

    void on_spnDecalDrawDistance_valueChanged(double arg1);

    void on_sldShadowDistInt_valueChanged(int value);

    void on_spnShadowDistInt_valueChanged(double arg1);

    void on_sldShadowDistExt_valueChanged(int value);

    void on_spnShadowDistExt_valueChanged(double arg1);

    void on_sldMouseSens_valueChanged(int value);

    void on_spnMouseSens_valueChanged(double arg1);

    void on_sldShadowBias_valueChanged(int value);

    void on_spnShadowBias_valueChanged(double arg1);

    void on_sldShadowDistFade_valueChanged(int value);

    void on_spnShadowDistFade_valueChanged(double arg1);

    void on_sldLightFadeDist_valueChanged(int value);

    void on_spnLightFadeDist_valueChanged(double arg1);

    void on_sldSpecFadeDist_valueChanged(int value);

    void on_spnSpecFadeDist_valueChanged(double arg1);

    void on_spnShadowMaskQuarter_valueChanged(int arg1);

    void on_sldShadowMaskQuarter_valueChanged(int value);

    void on_sldGrassDrawDistance_valueChanged(int value);

    void on_spnGrassDrawDistance_valueChanged(double arg1);

    void on_sldTreeDrawDistance_valueChanged(int value);

    void on_spnTreeDrawDistance_valueChanged(double arg1);

    void on_sldModelDrawDistance_valueChanged(int value);

    void on_spnModelDrawDistance_valueChanged(double arg1);

    void on_btnReset_clicked();

    void on_sldWaterMS_valueChanged(int value);

    void on_cmbShadowRes_currentIndexChanged(int index);

    void on_cmbWaterRes_currentIndexChanged(int index);

    void on_sldSunUpdate_valueChanged(int value);

    void on_spnSunUpdate_valueChanged(double arg1);

    void on_sldSunThres_valueChanged(int value);

    void on_spnSunThres_valueChanged(double arg1);

    void on_sldTreeLoadDistance_valueChanged(int value);

    void on_spnTreeLoadDistance_valueChanged(double arg1);

    void on_sldGrids_valueChanged(int value);

    void on_spnGrids_valueChanged(int arg1);

    void on_sldPreview_valueChanged(int value);

    void on_btnReload_clicked();

    void on_sldFOV_valueChanged(int value);

    void on_ckbReflect_Objects_toggled(bool checked);

    void on_ckbReflect_Trees_toggled(bool checked);

    void on_ckbReflect_Land_toggled(bool checked);

    void on_ckbReflect_Sky_toggled(bool checked);

    void on_cmbShadowQuality_simple_currentIndexChanged(int index);

    void on_sldMouseSCalX_valueChanged(int value);

    void on_spnMouseSCalX_valueChanged(double arg1);

    void on_sldMouseSCalY_valueChanged(int value);

    void on_spnMouseSCalY_valueChanged(double arg1);

    void on_sldHUDOpacity_valueChanged(int value);

    void on_spnHUDOpacity_valueChanged(double arg1);

    void on_sldBlockDistance_valueChanged(int value);

    void on_spnBlockDistance_valueChanged(double arg1);

    void on_sldSplitDistanceMul_valueChanged(int value);

    void on_spnSplitDistanceMul_valueChanged(double arg1);

    void on_sldSkyCellFadeDist_valueChanged(int value);

    void on_spnSkyCellFadeDist_valueChanged(double arg1);

    void on_sldNearDistance_valueChanged(int value);

    void on_spnNearDistance_valueChanged(double arg1);

private:
    Ui::MainWindow *ui;
    CSkyrimConfig *conf;
    CINI_Parse *toolConf;
    backups *backup_window;
    saves_profname *profname_window;
    bool spwin_open;
    bool buwin_open;
    QString base_path;

    bool init;

    QFile *pPrevF;
    CPreviewParser *pPreviews;

    void setDefaultValue(QSlider *sld, QString value, int def, int conv = 10000);
    void setDefaultValue(QDoubleSpinBox *spn, QString value, double def);
    void setDefaultValue(QSpinBox *spn, QString value, int def, int conv = 10000);
    void setDefaultValue(QDoubleSpinBox *spn, QString value, double def, QSlider *sld, int conv = 10000);
    void setDefaultValue(QSpinBox *spn, QString value, int def, QSlider *sld, int conv = 10000);
    void setDefaultValue(QLineEdit *txt, QString value, QString def);
    void setDefaultValue(QComboBox *cmb, QString value, int def);
    void setDefaultValue(QCheckBox *ckb, QString value, int def);
    void setDefaultValue(QGroupBox *ckb, QString value, int def);
    void setDefaultValues();
    void setPreview(QString image1);
    void setPreview(QString image1, QString image2);

    void PreviewHandler(QString element, int value);

    void closeEvent(QCloseEvent *event);

private: // actions/menus

    QAction *actSave;
    QAction *actReset;
    QAction *actReload;
    QAction *actImport;
    QAction *actExport;
    QAction *actExit;
    QAction *actAbout;

    QMenu *menFile;

    void createActions();
    void createMenus();
};

#endif // MAINWINDOW_H
