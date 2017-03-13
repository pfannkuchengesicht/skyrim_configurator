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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "backups.h"
#include <math.h>
#include <QCloseEvent>
#include <QFileDialog>
#include <ShlObj.h>
#include <QXmlStreamWriter>
#include "clog.h"
#include "cpreviewparser.h"
#include "version.h"
#include "exporter.h"
#include "importer.h"
#include "about.h"
#include "helpers.h"

#ifdef __PREVIEWS_EX__
    QString preview_base = ".";
#else
    QString preview_base = ":";
#endif

QStringList ftt_res;
QStringList stn_res;
QStringList stt_res;

MainWindow::MainWindow(QWidget *parent, bool log) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    init = true;

    if(log)
        skrmclog->enable();
    else
        skrmclog->disable();

#ifdef __PREVIEWS_EX__
    skrmclog->event(LOGLV_NORMAL, "App version: "+QString(VER_FILEVERSION_STR)+" external previews");
#else
    skrmclog->event(LOGLV_NORMAL, "App version: "+QString(VER_FILEVERSION_STR)+" internal previews");
#endif

    skrmclog->event(LOGLV_NORMAL, "initializing UI");

    ftt_res << "800x600" << "1024x768" << "1152x864" << "1280x1024" << "1280x960" << "1400x1050" << "1600x1200";
    stn_res << "1152x648" << "1280x720" << "1360x768" << "1366x768" << "1440x810" << "1600x900" << "1680x945" << "1920x1080" << "2560x1440";
    stt_res << "1024x640" << "1152x720" << "1280x800" << "1400x875" << "1440x900" << "1680x1050" << "1920x1200" << "2560x1600";

    ui->setupUi(this);
    ui->retranslateUi(this);

    createActions();
    createMenus();

#ifdef __PREVIEWS_EX__
    QDir test;
    test.setPath("previews");
    if(!test.exists())
    {
        ui->ckbShowPreview->setEnabled(false);
        skrmclog->event(LOGLV_WARNING, "could not find \"./previews\"!");
    }
#endif

    TCHAR szPath[MAX_PATH];
    SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, szPath);
    base_path = TCHAR2QString(szPath);
    base_path.append("\\My Games\\Skyrim\\");

    buwin_open = false;
    spwin_open = false;
    //this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    //Qt::WindowFlags flags = windowFlags() | Qt::MSWindowsFixedSizeDialogHint;
    //overrideWindowFlags( flags );

    ui->lblVersion->setText(VER_PRODUCTVERSION_STR);

    ui->spnGamma->setSingleStep(0.05);
    ui->spnMouseSens->setSingleStep(0.0005);
    ui->spnShadowBias->setSingleStep(0.005);
    ui->spnSunThres->setSingleStep(0.0005);
    ui->spnSunUpdate->setSingleStep(0.0005);
    ui->spnMouseSCalX->setSingleStep(0.0005);
    ui->spnMouseSCalY->setSingleStep(0.0005);

    ui->sldFOV->setValue(85);

    toolConf = new CINI_Parse("./config.ini");

    conf = new CSkyrimConfig();

    setDefaultValues();



    ui->ckbShowPreview->setChecked(true);
    setPreview("000.jpg");
    ui->ckbShowPreview->setChecked(false);
    this->adjustSize();
/*
    this->updateGeometry();
    this->update();
    QRect winGeom = this->geometry();
    QRect newGeom = QRect(winGeom.left(), winGeom.top(), winGeom.width() - 415, winGeom.height());
    this->setGeometry(newGeom);
    this->updateGeometry();
    this->update();
*/
    pPrevF = new QFile(preview_base + "/" + "previews.xml");

    if(pPrevF->open(QIODevice::ReadOnly))
    {
        pPreviews = new CPreviewParser(pPrevF);
        pPreviews->parse();
    }
    else
    {
        skrmclog->event(LOGLV_NORMAL, "could not find previews.xml");
        ui->ckbShowPreview->setEnabled(false);
    }

    //value bindings

    connect(ui->spnGamma, SIGNAL(valueChanged(double)), this, SLOT(spnGamma_change()));
    connect(ui->ckbFPRT, SIGNAL(clicked()), this, SLOT(ckbFPRT_change()));
    connect(ui->ckbScreenFuScr, SIGNAL(clicked()), this, SLOT(ckbScreenFuScr_change()));
    connect(ui->ckbVSync, SIGNAL(clicked()), this, SLOT(ckbVSync_change()));
    connect(ui->txtScreenResH, SIGNAL(textChanged(QString)), this, SLOT(txtScreenresH_change()));
    connect(ui->txtScreenResW, SIGNAL(textChanged(QString)), this, SLOT(txtScreenresW_change()));
    connect(ui->spnFOV, SIGNAL(valueChanged(int)), this, SLOT(spnFOV_change()));
    connect(ui->ckbFXAA, SIGNAL(clicked()), this, SLOT(ckbFXAA_change()));
    connect(ui->ckbTransAA, SIGNAL(clicked()), this, SLOT(ckbTransAA()));
    connect(ui->sldAA, SIGNAL(valueChanged(int)), this, SLOT(sldAA_change()));
    connect(ui->cmbRadBlurLev, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbRadBlurLev_change()));
    connect(ui->ckbDOF, SIGNAL(clicked()), this, SLOT(ckbDOF_change()));
    connect(ui->spnHUDOpacity, SIGNAL(valueChanged(double)), this, SLOT(spnHUDOpacity_change()));
    connect(ui->ckbCompass, SIGNAL(clicked()), this, SLOT(ckbCompass_change()));
    connect(ui->ckbQuestMarkers, SIGNAL(clicked()), this, SLOT(ckbQuestMarkers_change()));
    connect(ui->ckbQuestMarkersFloat, SIGNAL(clicked()), this, SLOT(ckbQuestMarkersFloat_change()));
    connect(ui->ckbSubtitles, SIGNAL(clicked()), this, SLOT(ckbSubtitles_change()));
    connect(ui->ckbSubtitlesDialogue, SIGNAL(clicked()), this, SLOT(ckbSubtitlesDialogue_change()));
    connect(ui->spnMaxParticles, SIGNAL(valueChanged(int)), this, SLOT(spnMaxParticles_change()));
    connect(ui->ckbThreadedParticles, SIGNAL(clicked()), this, SLOT(ckbThreadedParticles_change()));
    connect(ui->cmbTexQual, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbTexQual_change()));
    connect(ui->sldTexFilter, SIGNAL(valueChanged(int)), this, SLOT(sldTexFilter_change()));
    connect(ui->ckbThreadedtex, SIGNAL(clicked()), this, SLOT(ckbThreadedTex_change()));
    connect(ui->spnMaxDecalsModelsPA, SIGNAL(valueChanged(int)), this, SLOT(spnMaxDecalsModelsPA_change()));
    connect(ui->spnMaxDecalsModels, SIGNAL(valueChanged(int)), this, SLOT(spnMaxDecalsModels_change()));
    connect(ui->spnMaxDecals, SIGNAL(valueChanged(int)), this, SLOT(spnMaxDecals_change()));
    connect(ui->spnDecalDrawDistance, SIGNAL(valueChanged(double)), this, SLOT(spnDecalDrawDistance_change()));
    connect(ui->ckbDrawDecals, SIGNAL(clicked()), this, SLOT(ckbDrawDecals_change()));
    connect(ui->ckbDrawDecalsModels, SIGNAL(clicked()), this, SLOT(ckbDrawDecalsmodels_change()));
    connect(ui->ckbThreadedDecals, SIGNAL(clicked()), this, SLOT(ckbThreadedDecals_change()));
    connect(ui->ckbBorderRegion, SIGNAL(clicked()), this, SLOT(ckbBorderRegion_change()));
    connect(ui->ckbThreadedLOD, SIGNAL(clicked()), this, SLOT(ckbThreadedLOD_change()));
    connect(ui->ckbThreadedMesh, SIGNAL(clicked()), this, SLOT(ckbThreadedMesh_change()));
    connect(ui->ckbThreadedMorpher, SIGNAL(clicked()), this, SLOT(ckbThreadedMorpher_change()));
    connect(ui->ckbUnloadOnFT, SIGNAL(clicked()), this, SLOT(ckbUnloadOnFT_change()));
    connect(ui->spnItmDist, SIGNAL(valueChanged(int)), this, SLOT(spnItmDist_change()));
    connect(ui->spnObjDist, SIGNAL(valueChanged(int)), this, SLOT(spnObjDist_change()));
    connect(ui->spnActDist, SIGNAL(valueChanged(int)), this, SLOT(spnActDist_change()));
    connect(ui->spnModelDrawDistance, SIGNAL(valueChanged(double)), this, SLOT(spnModelDrawDistance_change()));
    connect(ui->spnGrids, SIGNAL(valueChanged(int)), this, SLOT(spnGrids_change()));
    connect(ui->spnNearDistance, SIGNAL(valueChanged(double)), this, SLOT(spnNearDistance_change()));
    connect(ui->spnSplitDistanceMul, SIGNAL(valueChanged(double)), this, SLOT(spnSplitDistanceMul_change()));
    connect(ui->spnSkyCellFadeDist, SIGNAL(valueChanged(double)), this, SLOT(spnSkyCellFadeDist_change()));
    connect(ui->spnBlockDistance, SIGNAL(valueChanged(double)), this, SLOT(spnBlockDistance_change()));
    connect(ui->spnAniTrees, SIGNAL(valueChanged(int)), this, SLOT(spnAniTrees_change()));
    connect(ui->spnTreeLoadDistance, SIGNAL(valueChanged(double)), this, SLOT(spnTreeLoadDistance_change()));
    connect(ui->ckbSkinnedTrees, SIGNAL(clicked()), this, SLOT(ckbSkinnedTrees_change()));
    connect(ui->ckbTreeThreads, SIGNAL(clicked()), this, SLOT(ckbTreeThreads_change()));
    connect(ui->spnTreeDrawDistance, SIGNAL(valueChanged(double)), this, SLOT(spnTreeDrawDistance_change()));
    connect(ui->spnGrassDensi, SIGNAL(valueChanged(int)), this, SLOT(spnGrassDensi_change()));
    connect(ui->spnGrassDrawDistance, SIGNAL(valueChanged(double)), this, SLOT(spnGrassDrawDistance_change()));
    connect(ui->ckbEnhancedGrass, SIGNAL(clicked()), this, SLOT(ckbEnhancedGrass_change()));
    connect(ui->spnShadowSplitC, SIGNAL(valueChanged(int)), this, SLOT(spnShadowSplitC_change()));
    connect(ui->cmbShadowMode, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbShadowMode_change()));
    connect(ui->spnShadowBlur, SIGNAL(valueChanged(int)), this, SLOT(spnShadowBlur_change()));
    connect(ui->cmbShadowRes, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbShadowRes_change()));
    connect(ui->spnShadowBias, SIGNAL(valueChanged(double)), this, SLOT(spnShadowBias_change()));
    connect(ui->cmbShadowFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbShadowFilter_change()));
    connect(ui->ckbDrawShadows, SIGNAL(clicked()), this, SLOT(ckbDrawShadows_change()));
    connect(ui->ckbPlrSelfShad, SIGNAL(clicked()), this, SLOT(ckbPlrSelfShad_change()));
    connect(ui->ckbTreeShadows, SIGNAL(clicked()), this, SLOT(ckbTreeShadows_change()));
    connect(ui->ckbGrassShadows, SIGNAL(clicked()), this, SLOT(ckbGrassShadows_change()));
    connect(ui->ckbLandShadows, SIGNAL(clicked()), this, SLOT(ckbLandShadows_change()));
    connect(ui->ckbTorchShadow, SIGNAL(clicked()), this, SLOT(ckbTorchShadows_change()));
    connect(ui->spnShadowDistExt, SIGNAL(valueChanged(double)), this, SLOT(spnShadowDistExt_change()));
    connect(ui->spnShadowDistInt, SIGNAL(valueChanged(double)), this, SLOT(spnShadowDistInt_change()));
    connect(ui->spnShadowMaskQuarter, SIGNAL(valueChanged(int)), this, SLOT(spnShadowMaskQuarter_change()));
    connect(ui->spnShadowDistFade, SIGNAL(valueChanged(double)), this, SLOT(spnShadowDistFade_change()));
    connect(ui->spnSunUpdate, SIGNAL(valueChanged(double)), this, SLOT(spnSunUpdate_change()));
    connect(ui->spnSpecFadeDist, SIGNAL(valueChanged(double)), this, SLOT(spnSpecFadeDist_change()));
    connect(ui->spnLightFadeDist, SIGNAL(valueChanged(double)), this, SLOT(spnLightFadeDist_change()));
    connect(ui->spnSunThres, SIGNAL(valueChanged(double)), this, SLOT(spnSunThres_change()));
    connect(ui->ckbSimpleLight, SIGNAL(clicked()), this, SLOT(ckbSimpleLight_change()));
    connect(ui->ckbSunBeams, SIGNAL(clicked()), this, SLOT(ckbSunBeams_change()));
    connect(ui->sldWaterMS, SIGNAL(valueChanged(int)), this, SLOT(sldWaterMS_change()));
    connect(ui->ckbDepth, SIGNAL(clicked()), this, SLOT(ckbDepth_change()));
    connect(ui->ckbDisplace, SIGNAL(clicked()), this, SLOT(ckbDisplace_change()));
    connect(ui->ckbRefract, SIGNAL(clicked()), this, SLOT(ckbRefract_change()));
    connect(ui->ckbWaterLOD, SIGNAL(clicked()), this, SLOT(ckbWaterLOD_change()));
    connect(ui->ckbWaterShader, SIGNAL(clicked()), this, SLOT(ckbWaterShader_change()));
    connect(ui->spnReflectBlur, SIGNAL(valueChanged(int)), this, SLOT(spnReflectBlur_change()));
    connect(ui->cmbWaterRes, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbWaterRes_change()));
    connect(ui->ckbReflect_Explosions, SIGNAL(clicked()), this, SLOT(ckbReflectExplosions_change()));
    connect(ui->ckbReflect_Sky, SIGNAL(clicked()), this, SLOT(ckbReflectSky_change()));
    connect(ui->ckbSilRefl, SIGNAL(clicked()), this, SLOT(ckbSilRefl_change()));
    connect(ui->ckbReflect_Land, SIGNAL(clicked()), this, SLOT(ckbReflectLand_change()));
    connect(ui->ckbReflect_Objects, SIGNAL(clicked()), this, SLOT(ckbReflectObjects_change()));
    connect(ui->ckbReflect_Trees, SIGNAL(clicked()), this, SLOT(ckbReflectTrees_change()));
    connect(ui->ckbForceHD, SIGNAL(clicked()), this, SLOT(ckbForceHD_change()));
    connect(ui->ckbReflectBlur, SIGNAL(clicked()), this, SLOT(ckbReflectBlur_change()));
    connect(ui->grpReflect, SIGNAL(clicked()), this, SLOT(grpReflect_change()));
    connect(ui->ckbMouseInvertY, SIGNAL(clicked()), this, SLOT(ckbMouseInvertY_change()));
    connect(ui->spnMouseSCalY, SIGNAL(valueChanged(double)), this, SLOT(spnMouseSCalY_change()));
    connect(ui->spnMouseSCalX, SIGNAL(valueChanged(double)), this, SLOT(spnMouseSCalX_change()));
    connect(ui->spnMouseSens, SIGNAL(valueChanged(double)), this, SLOT(spnMouseSens_change()));
    connect(ui->ckbMouseAccel, SIGNAL(clicked()), this, SLOT(ckbMouseAccel_change()));
    connect(ui->ckbAlwaysActive, SIGNAL(clicked()), this, SLOT(ckbAlwaysActive_change()));
    connect(ui->ckbIntro, SIGNAL(clicked()), this, SLOT(ckbIntro_change()));
    connect(ui->ckbScreenshot, SIGNAL(clicked()), this, SLOT(ckbScreenshot_change()));
    connect(ui->ckbFileSelection, SIGNAL(clicked()), this, SLOT(ckbFileSelection_change()));
    connect(ui->ckbSavePause, SIGNAL(clicked()), this, SLOT(ckbSavePause_change()));
    connect(ui->ckbSaveWait, SIGNAL(clicked()), this, SLOT(ckbSaveWait_change()));
    connect(ui->ckbSaveRest, SIGNAL(clicked()), this, SLOT(ckbSaveRest_change()));
    connect(ui->ckbSaveTravel, SIGNAL(clicked()), this, SLOT(ckbSaveTravel_change()));
    connect(ui->spnAutoSaveInt, SIGNAL(valueChanged(int)), this, SLOT(spnAutoSaveInt_change()));
    connect(ui->grpAutoSave, SIGNAL(clicked()), this, SLOT(grpAutoSave_change()));
    connect(ui->ckbBckgrndLoadLip, SIGNAL(clicked()), this, SLOT(ckbBckgrndLoadLip()));
    connect(ui->ckbBckgrndLoadFaceGen, SIGNAL(clicked()), this, SLOT(ckbBckgrndLoadFaceGen()));

    //preview bindings
    //screen
    connect(ui->sldAA, SIGNAL(valueChanged(int)), this, SLOT(PreviewHandlerInt(int)));
    connect(ui->ckbTransAA, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->ckbFXAA, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->ckbDOF, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->ckbVSync, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->sldFOV, SIGNAL(valueChanged(int)), this, SLOT(PreviewHandlerInt(int)));
    connect(ui->sldGamma, SIGNAL(valueChanged(int)), this, SLOT(PreviewHandlerInt(int)));
    connect(ui->cmbRadBlurLev, SIGNAL(currentIndexChanged(int)), this, SLOT(PreviewHandlerInt(int)));

    //tex
    connect(ui->sldTexFilter, SIGNAL(valueChanged(int)), this, SLOT(PreviewHandlerInt(int)));
    connect(ui->cmbTexQual, SIGNAL(currentIndexChanged(int)), this, SLOT(PreviewHandlerInt(int)));

    //particles
    connect(ui->sldMaxParticles, SIGNAL(valueChanged(int)), this, SLOT(PreviewHandlerInt(int)));

    //decals
    connect(ui->sldMaxDecals, SIGNAL(valueChanged(int)), this, SLOT(PreviewHandlerInt(int)));
    connect(ui->sldMaxDecalsModels, SIGNAL(valueChanged(int)), this, SLOT(PreviewHandlerInt(int)));
    connect(ui->sldMaxDecalsModelsPA, SIGNAL(valueChanged(int)), this, SLOT(PreviewHandlerInt(int)));
    connect(ui->spnDecalDrawDistance, SIGNAL(valueChanged(double)), this, SLOT(PreviewHandlerDblH(double)));
    connect(ui->ckbDrawDecals, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->ckbDrawDecalsModels, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));

    //model dists
    connect(ui->sldActDist, SIGNAL(valueChanged(int)), this, SLOT(PreviewHandlerInt(int)));
    connect(ui->sldItmDist, SIGNAL(valueChanged(int)), this, SLOT(PreviewHandlerInt(int)));
    connect(ui->sldObjDist, SIGNAL(valueChanged(int)), this, SLOT(PreviewHandlerInt(int)));
    connect(ui->spnModelDrawDistance, SIGNAL(valueChanged(double)), this, SLOT(PreviewHandlerDblH(double)));
    connect(ui->ckbBorderRegion, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));

    //grids
    connect(ui->sldGrids, SIGNAL(valueChanged(int)), this, SLOT(PreviewHandlerInt(int)));

    //world
    connect(ui->spnBlockDistance, SIGNAL(valueChanged(double)), this, SLOT(PreviewHandlerDblH(double)));
    connect(ui->spnSplitDistanceMul, SIGNAL(valueChanged(double)), this, SLOT(PreviewHandlerDblH(double)));
    connect(ui->spnSkyCellFadeDist, SIGNAL(valueChanged(double)), this, SLOT(PreviewHandlerDblH(double)));
    connect(ui->spnNearDistance, SIGNAL(valueChanged(double)), this, SLOT(PreviewHandlerDblH(double)));

    //grass
    connect(ui->sldGrassDensi, SIGNAL(valueChanged(int)), this, SLOT(PreviewHandlerInt(int)));
    connect(ui->sldGrassDrawDistance, SIGNAL(valueChanged(int)), this, SLOT(PreviewHandlerInt(int)));
    connect(ui->ckbEnhancedGrass, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));

    //trees
    connect(ui->spnTreeDrawDistance, SIGNAL(valueChanged(double)), this, SLOT(PreviewHandlerDblH(double)));
    connect(ui->spnTreeLoadDistance, SIGNAL(valueChanged(double)), this, SLOT(PreviewHandlerDblH(double)));
    connect(ui->ckbSkinnedTrees, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->sldAniTrees, SIGNAL(valueChanged(int)), this, SLOT(PreviewHandlerInt(int)));

    //light
    connect(ui->spnLightFadeDist, SIGNAL(valueChanged(double)), this, SLOT(PreviewHandlerDblH(double)));
    connect(ui->spnSpecFadeDist, SIGNAL(valueChanged(double)), this, SLOT(PreviewHandlerDblH(double)));
    connect(ui->ckbSunBeams, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->ckbSimpleLight, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));

    //shadows
    connect(ui->sldShadowMaskQuarter, SIGNAL(valueChanged(int)), this, SLOT(PreviewHandlerInt(int)));
    connect(ui->spnShadowDistExt, SIGNAL(valueChanged(double)), this, SLOT(PreviewHandlerDblH(double)));
    connect(ui->spnShadowDistInt, SIGNAL(valueChanged(double)), this, SLOT(PreviewHandlerDblH(double)));
    connect(ui->spnShadowDistFade, SIGNAL(valueChanged(double)), this, SLOT(PreviewHandlerDblH(double)));
    connect(ui->spnShadowBias, SIGNAL(valueChanged(double)), this, SLOT(PreviewHandlerDblL(double)));
    connect(ui->cmbShadowRes, SIGNAL(currentIndexChanged(int)), this, SLOT(PreviewHandlerInt(int)));
    connect(ui->cmbShadowFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(PreviewHandlerInt(int)));
    connect(ui->sldShadowBlur, SIGNAL(valueChanged(int)), this, SLOT(PreviewHandlerInt(int)));
    connect(ui->sldShadowSplitC, SIGNAL(valueChanged(int)), this, SLOT(PreviewHandlerInt(int)));
    connect(ui->ckbDrawShadows, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->cmbShadowMode, SIGNAL(currentIndexChanged(int)), this, SLOT(PreviewHandlerInt(int)));
    connect(ui->ckbTreeShadows, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->ckbPlrSelfShad, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->ckbLandShadows, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->ckbTorchShadow, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->cmbShadowQuality_simple, SIGNAL(currentIndexChanged(int)), this, SLOT(PreviewHandlerInt(int)));
    connect(ui->ckbShadowLOD, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->ckbGrassShadows, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));

    //water
    connect(ui->cmbWaterRes, SIGNAL(currentIndexChanged(int)), this, SLOT(PreviewHandlerInt(int)));
    connect(ui->ckbWaterShader, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->ckbRefract, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->ckbDisplace, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->ckbDepth, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->ckbWaterLOD, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->ckbForceHD, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->grpReflect, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->sldReflectBlur, SIGNAL(valueChanged(int)), this, SLOT(PreviewHandlerInt(int)));
    connect(ui->sldWaterMS, SIGNAL(valueChanged(int)), this, SLOT(PreviewHandlerInt(int)));

    //UI
    connect(ui->ckbCompass, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->ckbQuestMarkers, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->ckbQuestMarkersFloat, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->ckbSubtitles, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->ckbSubtitlesDialogue, SIGNAL(toggled(bool)), this, SLOT(PreviewHandlerBool(bool)));
    connect(ui->sldHUDOpacity, SIGNAL(valueChanged(int)), this, SLOT(PreviewHandlerInt(int)));

    init = false;
}

MainWindow::~MainWindow()
{
    delete conf;
    delete ui;
}

void MainWindow::on_cmbScreenRes_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    if(ui->cmbScreenRes->currentText().isEmpty())
        return;

    QStringList scrRes;

    scrRes = ui->cmbScreenRes->currentText().split("x");

    ui->txtScreenResH->setText(scrRes.at(1));
    ui->txtScreenResW->setText(scrRes.at(0));
}

void MainWindow::on_ckbUseCustom_toggled(bool checked)
{
    if(checked)
    {
        ui->cmbScreenRes->setEnabled(false);
        ui->txtScreenResH->setEnabled(true);
        ui->txtScreenResW->setEnabled(true);
    }
    else
    {
        ui->cmbScreenRes->setEnabled(true);
        ui->txtScreenResH->setEnabled(false);
        ui->txtScreenResW->setEnabled(false);
    }
}

void MainWindow::on_sldAA_valueChanged(int value)
{
    ui->lblMSAAIndi->setText(QString::number(ipow(2, value)) + "x");
    /*
    switch(value)
    {
    case 0:
        ui->lblMSAAIndi->setText("0x");
        break;
    case 1:
        ui->lblMSAAIndi->setText("2x");
        break;
    case 2:
        ui->lblMSAAIndi->setText("4x");
        break;
    case 3:
        ui->lblMSAAIndi->setText("8x");
        break;
    }
    */
}

void MainWindow::on_ckbShowPreview_toggled(bool checked)
{
    if(checked)
    {
        //this->setFixedWidth(984);
        //this->setMinimumWidth(this->width() + 424);
        ui->grpPreview->setVisible(true);
        this->adjustSize();
        //QRect winGeom = this->geometry();
        //QRect newGeom = QRect(winGeom.left(), winGeom.top(), winGeom.width() + 415, winGeom.height());
        //this->setGeometry(newGeom);
    }
    else
    {
        //this->setFixedWidth(560);
        //this->setMinimumWidth(this->width() + 424);
        ui->grpPreview->setVisible(false);
        this->adjustSize();
        //this->updateGeometry();
        //this->update();

        //QRect winGeom = this->geometry();
        //QRect newGeom = QRect(winGeom.left(), winGeom.top(), winGeom.width() - 415, winGeom.height());
        //this->setGeometry(newGeom);

        //this->updateGeometry();
        //this->update();
    }
}

void MainWindow::on_sldGamma_valueChanged(int value)
{
    ui->spnGamma->setValue(static_cast<double>(value)/1000.f);
}

void MainWindow::on_btnApply_clicked()
{
    skrmclog->event(LOGLV_NORMAL, "applying settings to config");
    conf->save();
    toolConf->save();

    skrmclog->event(LOGLV_NORMAL, "reloading configs.");
    delete conf;
    conf = new CSkyrimConfig();
}

void MainWindow::on_cmbAspect_currentIndexChanged(int index)
{
    ui->cmbScreenRes->clear();
    switch(index)
    {
    case 0:
        ui->cmbScreenRes->addItems(ftt_res);
        break;
    case 1:
        ui->cmbScreenRes->addItems(stn_res);
        break;
    case 2:
        ui->cmbScreenRes->addItems(stt_res);
    }
}

void MainWindow::setDefaultValues()
{
    skrmclog->event(LOGLV_NORMAL, "loading default values");
    conf->startSection("Display");

    setDefaultValue(ui->spnGamma, conf->getValue("fGamma"), 1.000);
    setDefaultValue(ui->ckbVSync, conf->getValue("iPresentInterval"), 0);
    setDefaultValue(ui->ckbTransAA, conf->getValue("bTransparencyMultisampling"), 0);
    setDefaultValue(ui->ckbScreenFuScr, conf->getValue("bFull Screen"), 1);
    setDefaultValue(ui->ckbFXAA, conf->getValue("bFXAAEnabled"), 0);
    setDefaultValue(ui->txtScreenResH, conf->getValue("iSize H"), "480");
    setDefaultValue(ui->txtScreenResW, conf->getValue("iSize W"), "640");

    if(ftt_res.contains(conf->getValue("iSize W") + "x" + conf->getValue("iSize H")))
        ui->cmbAspect->setCurrentIndex(0);
    else if(stn_res.contains(conf->getValue("iSize W") + "x" + conf->getValue("iSize H")))
        ui->cmbAspect->setCurrentIndex(1);
    else if(stt_res.contains(conf->getValue("iSize W") + "x" + conf->getValue("iSize H")))
        ui->cmbAspect->setCurrentIndex(2);

    ui->cmbScreenRes->setCurrentIndex(ui->cmbScreenRes->findText(conf->getValue("iSize W") + "x" + conf->getValue("iSize H")));

    switch(conf->getValue("iMultiSample").toInt())
    {
    case 1:
        ui->sldAA->setValue(0);
        break;
    case 2:
        ui->sldAA->setValue(1);
        break;
    case 4:
        ui->sldAA->setValue(2);
        break;
    case 8:
        ui->sldAA->setValue(3);
        break;
    }

    switch(conf->getValue("iMaxAnisotropy").toInt())
    {
    case 0:
        ui->sldTexFilter->setValue(0);
        break;
    case 2:
        ui->sldTexFilter->setValue(1);
        break;
    case 4:
        ui->sldTexFilter->setValue(2);
        break;
    case 8:
        ui->sldTexFilter->setValue(3);
        break;
    case 16:
        ui->sldTexFilter->setValue(4);
        break;
    }

    setDefaultValue(ui->cmbTexQual, QVariant(4 - conf->getValue("iTexMipMapSkip").toInt()).toString(), 0);
    setDefaultValue(ui->spnDecalDrawDistance, conf->getValue("fDecalLOD1"), 1000.0f, ui->sldDecalDrawDistance, 1000);
    setDefaultValue(ui->ckbDrawShadows, conf->getValue("bDrawShadows"), 1);

    switch(conf->getValue("iShadowMapResolution").toInt())
    {
    case 256:
        ui->cmbShadowRes->setCurrentIndex(0);
        break;
    case 512:
        ui->cmbShadowRes->setCurrentIndex(1);
        break;
    case 1024:
        ui->cmbShadowRes->setCurrentIndex(2);
        break;
    case 2048:
        ui->cmbShadowRes->setCurrentIndex(3);
        break;
    case 4096:
        ui->cmbShadowRes->setCurrentIndex(4);
        break;
    case 8192:
        ui->cmbShadowRes->setCurrentIndex(5);
        break;
    default:
        ui->cmbShadowRes->setCurrentIndex(2);
        break;
    }

    setDefaultValue(ui->cmbShadowFilter, QVariant(conf->getValue("iShadowFilter").toInt() - 1).toString(), 0);
    setDefaultValue(ui->spnShadowBlur, conf->getValue("iBlurDeferredShadowMask"), 0, ui->sldShadowBlur, 1);
    setDefaultValue(ui->cmbShadowMode, QVariant(conf->getValue("iShadowMode").toInt() - 1).toString(), 2);
    setDefaultValue(ui->spnShadowDistExt, conf->getValue("fShadowDistance"), 4000.0f, ui->sldShadowDistExt, 1000);
    setDefaultValue(ui->spnShadowDistInt, conf->getValue("fInteriorShadowDistance"), 4000.0f, ui->sldShadowDistInt, 1000);
    setDefaultValue(ui->ckbLandShadows, conf->getValue("bDrawLandShadows"), 0);
    setDefaultValue(ui->ckbTreeShadows, conf->getValue("bTreesReceiveShadows"), 0);
    setDefaultValue(ui->ckbPlrSelfShad, conf->getValue("bActorSelfShadowing"), 0);
    setDefaultValue(ui->ckbTorchShadow, conf->getValue("bEquippedTorchesCastShadows"), 0);
    setDefaultValue(ui->ckbGrassShadows, conf->getValue("bShadowsOnGrass"), 0);

    if(conf->getValue("iShadowMapResolutionPrimary").toInt() == conf->getValue("iShadowMapResolutionSecondary").toInt())
    {
        setDefaultValue(ui->ckbShadowLOD, "0", 1);
    }
    else
    {
        setDefaultValue(ui->ckbShadowLOD, "1", 1);
    }

    setDefaultValue(ui->ckbSimpleLight, conf->getValue("bSimpleLighting"), 0);
    setDefaultValue(ui->ckbSunBeams, conf->getValue("bUseSunbeams"), 1);
    setDefaultValue(ui->spnLightFadeDist, conf->getValue("fLightLODStartFade"), 2500.0f, ui->sldLightFadeDist, 1000);
    setDefaultValue(ui->spnSpecFadeDist, conf->getValue("fSpecularLODStartFade"), 1000.0f, ui->sldSpecFadeDist, 1000);
    setDefaultValue(ui->spnShadowBias, conf->getValue("fShadowBiasScale"), 0.5f, ui->sldShadowBias, 10000);
    setDefaultValue(ui->spnShadowDistFade, conf->getValue("fShadowLODStartFade"), 200.0f, ui->sldShadowDistFade, 1000);
    setDefaultValue(ui->spnShadowSplitC, conf->getValue("iShadowSplitCount"), 2, ui->sldShadowSplitC, 1);
    setDefaultValue(ui->spnShadowMaskQuarter, conf->getValue("iShadowMaskQuarter"), 4, ui->sldShadowMaskQuarter, 1);

    if(ui->cmbShadowRes->currentIndex() == 0 &&
       ui->cmbShadowFilter->currentIndex() == 0 &&
       ui->spnShadowBlur->value() == 5 &&
       ui->spnShadowDistExt->value() == 1500.0 &&
       ui->spnShadowDistInt->value() == 2000.0 &&
       ui->spnShadowBias->value() == 0.75 &&
       ui->spnShadowMaskQuarter->value() == 1 &&
       !ui->ckbLandShadows->isChecked() &&
       !ui->ckbTreeShadows->isChecked() &&
       !ui->ckbTorchShadow->isChecked() &&
       !ui->ckbPlrSelfShad->isChecked() &&
       !ui->ckbGrassShadows->isChecked() &&
       ui->ckbShadowLOD->isChecked()
       )
    {
        ui->cmbShadowQuality_simple->setCurrentIndex(0);
    }
    else if(ui->cmbShadowRes->currentIndex() == 1 &&
            ui->cmbShadowFilter->currentIndex() == 1 &&
            ui->spnShadowBlur->value() == 3 &&
            ui->spnShadowDistExt->value() == 2000.0 &&
            ui->spnShadowDistInt->value() == 4000.0 &&
            ui->spnShadowBias->value() == 0.6 &&
            ui->spnShadowMaskQuarter->value() == 2 &&
            !ui->ckbLandShadows->isChecked() &&
            !ui->ckbTreeShadows->isChecked() &&
            !ui->ckbTorchShadow->isChecked() &&
            !ui->ckbPlrSelfShad->isChecked() &&
            !ui->ckbGrassShadows->isChecked() &&
            ui->ckbShadowLOD->isChecked()
            )
    {
        ui->cmbShadowQuality_simple->setCurrentIndex(1);
    }
    else if(ui->cmbShadowRes->currentIndex() == 2 &&
            ui->cmbShadowFilter->currentIndex() == 2 &&
            ui->spnShadowBlur->value() == 1 &&
            ui->spnShadowDistExt->value() == 3500.0 &&
            ui->spnShadowDistInt->value() == 4000.0 &&
            ui->spnShadowBias->value() == 0.5 &&
            ui->spnShadowMaskQuarter->value() == 3 &&
            ui->ckbLandShadows->isChecked() &&
            ui->ckbTreeShadows->isChecked() &&
            !ui->ckbTorchShadow->isChecked() &&
            !ui->ckbPlrSelfShad->isChecked() &&
            !ui->ckbGrassShadows->isChecked() &&
            ui->ckbShadowLOD->isChecked()
            )
    {
        ui->cmbShadowQuality_simple->setCurrentIndex(2);
    }
    else if(ui->cmbShadowRes->currentIndex() == 3 &&
            ui->cmbShadowFilter->currentIndex() == 3 &&
            ui->spnShadowBlur->value() == 1 &&
            ui->spnShadowDistExt->value() == 4000.0 &&
            ui->spnShadowDistInt->value() == 4000.0 &&
            ui->spnShadowBias->value() == 0.5 &&
            ui->spnShadowMaskQuarter->value() == 5 &&
            ui->ckbLandShadows->isChecked() &&
            ui->ckbTreeShadows->isChecked() &&
            ui->ckbTorchShadow->isChecked() &&
            ui->ckbPlrSelfShad->isChecked() &&
            ui->ckbGrassShadows->isChecked() &&
            ui->ckbShadowLOD->isChecked()
            )
    {
        ui->cmbShadowQuality_simple->setCurrentIndex(3);
    }
    else if(ui->cmbShadowRes->currentIndex() == 4 &&
            ui->cmbShadowFilter->currentIndex() == 3 &&
            ui->spnShadowBlur->value() == 1 &&
            ui->spnShadowDistExt->value() == 6000.0 &&
            ui->spnShadowDistInt->value() == 4000.0 &&
            ui->spnShadowBias->value() == 0.35 &&
            ui->spnShadowMaskQuarter->value() == 8 &&
            ui->ckbLandShadows->isChecked() &&
            ui->ckbTreeShadows->isChecked() &&
            ui->ckbTorchShadow->isChecked() &&
            ui->ckbPlrSelfShad->isChecked() &&
            ui->ckbGrassShadows->isChecked() &&
            !ui->ckbShadowLOD->isChecked()
            )
    {
        ui->cmbShadowQuality_simple->setCurrentIndex(4);
    }
    else if(ui->cmbShadowRes->currentIndex() == 5 &&
            ui->cmbShadowFilter->currentIndex() == 3 &&
            ui->spnShadowBlur->value() == 1 &&
            ui->spnShadowDistExt->value() == 7000.0 &&
            ui->spnShadowDistInt->value() == 4000.0 &&
            ui->spnShadowBias->value() == 0.3 &&
            ui->spnShadowMaskQuarter->value() == 10 &&
            ui->ckbLandShadows->isChecked() &&
            ui->ckbTreeShadows->isChecked() &&
            ui->ckbTorchShadow->isChecked() &&
            ui->ckbPlrSelfShad->isChecked() &&
            ui->ckbGrassShadows->isChecked() &&
            !ui->ckbShadowLOD->isChecked()
            )
    {
        ui->cmbShadowQuality_simple->setCurrentIndex(5);
    }
    else
    {
        ui->cmbShadowQuality_simple->setCurrentIndex(6);
    }

    setDefaultValue(ui->spnModelDrawDistance, conf->getValue("fMeshLODLevel1FadeDist"), 10000.0f, ui->sldModelDrawDistance, 10);
    setDefaultValue(ui->spnTreeDrawDistance, conf->getValue("fMeshLODLevel1FadeTreeDistance"), 2844.0f, ui->sldTreeDrawDistance, 1000);

    switch(conf->getValue("iWaterMultiSamples").toInt())
    {
    case 0:
        ui->sldWaterMS->setValue(0);
        break;
    case 2:
        ui->sldWaterMS->setValue(1);
        break;
    case 4:
        ui->sldWaterMS->setValue(2);
        break;
    case 8:
        ui->sldWaterMS->setValue(3);
        break;
    }

    setDefaultValue(ui->spnSunThres, conf->getValue("fSunUpdateThreshold"), 0.5f);
    setDefaultValue(ui->spnSunUpdate, conf->getValue("fSunShadowUpdateTime"), 1.0f);

    setDefaultValue(ui->ckbScreenshot, conf->getValue("bAllowScreenshot"), 1);

    setDefaultValue(ui->spnNearDistance, conf->getValue("fNearDistance"), 15.0f, ui->sldNearDistance);

    setDefaultValue(ui->ckbFPRT, conf->getValue("bFloatPointRenderTarget"), 0);

    conf->endSection();
    conf->startSection("LOD");

        setDefaultValue(ui->spnObjDist, QVariant((int)conf->getValue("fLODFadeOutMultObjects").toFloat()).toString(), 15, ui->sldObjDist, 1);
        setDefaultValue(ui->spnItmDist, QVariant((int)conf->getValue("fLODFadeOutMultItems").toFloat()).toString(), 15, ui->sldItmDist, 1);
        setDefaultValue(ui->spnActDist, QVariant((int)conf->getValue("fLODFadeOutMultActors").toFloat()).toString(), 20, ui->sldActDist, 1);

    conf->endSection();
    conf->startSection("Imagespace");

    setDefaultValue(ui->ckbDOF, conf->getValue("bDoDepthOfField"), 1);
    setDefaultValue(ui->cmbRadBlurLev, conf->getValue("iRadialBlurLevel"), 1);
    ui->cmbRadBlurLev->setCurrentIndex(ui->cmbRadBlurLev->currentIndex() + 1);
    if(conf->getValue("bDoRadialBlur").toInt() == 0 && conf->getValue("bDoRadialBlur") != "NULL")
    {
        ui->cmbRadBlurLev->setCurrentIndex(0);
    }

    conf->endSection();
    conf->startSection("BackgroundLoad");

    setDefaultValue(ui->ckbUnloadOnFT, conf->getValue("bSelectivePurgeUnusedOnFastTravel"), 0);
    setDefaultValue(ui->ckbBckgrndLoadLip, conf->getValue("bBackgroundLoadLipFiles"), 0);
    setDefaultValue(ui->ckbBckgrndLoadFaceGen, conf->getValue("bLoadBackgroundFaceGen"), 0);

    conf->endSection();
    conf->startSection("General");

    setDefaultValue(ui->sldFOV, conf->getValue("fDefaultWorldFOV"), 67, 1);

    setDefaultValue(ui->ckbThreadedParticles, conf->getValue("bUseThreadedParticleSystem"), 0);
    setDefaultValue(ui->ckbThreadedMorpher, conf->getValue("bUseThreadedMorpher"), 0);
    setDefaultValue(ui->ckbThreadedMesh, conf->getValue("bUseThreadedMeshes"), 0);
    setDefaultValue(ui->ckbThreadedLOD, conf->getValue("bUseThreadedLOD"), 0);
    setDefaultValue(ui->ckbThreadedtex, conf->getValue("bUseThreadedTextures"), 0);
    setDefaultValue(ui->spnGrids, conf->getValue("uGridsToLoad"), 5, 1);
    setDefaultValue(ui->ckbBorderRegion, conf->getValue("bBorderRegionsEnabled"), 1);

    if(conf->getValue("sIntroSequence") == "BGS_LOGO.BIK")
    {
        ui->ckbIntro->setChecked(true);
    }
    else if(conf->getValue("sIntroSequence").isEmpty())
    {
        ui->ckbIntro->setChecked(false);
    }
    else
    {
        ui->ckbIntro->setChecked(true);
    }

    setDefaultValue(ui->ckbAlwaysActive, conf->getValue("bAlwaysActive"), 0);

    conf->endSection();
    conf->startSection("Decals");

    setDefaultValue(ui->ckbDrawDecals, conf->getValue("bDecals"), 1);
    setDefaultValue(ui->ckbDrawDecalsModels, conf->getValue("bSkinnedDecals"), 1);
    setDefaultValue(ui->spnMaxDecals, conf->getValue("uMaxDecals"), 250, ui->sldMaxDecals, 1);
    setDefaultValue(ui->spnMaxDecalsModels, conf->getValue("uMaxSkinDecals"), 50, ui->sldMaxDecalsModels, 1);
    setDefaultValue(ui->spnMaxDecalsModelsPA, conf->getValue("uMaxSkinDecalsPerActor"), 40, ui->sldMaxDecalsModelsPA, 1);
    setDefaultValue(ui->ckbThreadedDecals, conf->getValue("bUseThreadedBlood"), 0);

    conf->endSection();
    conf->startSection("Particles");

    setDefaultValue(ui->sldMaxParticles, conf->getValue("iMaxDesired"), 750, 1);

    conf->endSection();
    conf->startSection("Trees");

    setDefaultValue(ui->ckbTreeThreads, conf->getValue("bUseMultiThreadedTrees"), 0);
    setDefaultValue(ui->ckbSkinnedTrees, conf->getValue("bEnableTreeAnimations"), 1);
    setDefaultValue(ui->spnAniTrees, conf->getValue("uiMaxSkinnedTreesToRender"), 20, ui->sldAniTrees, 1);
    setDefaultValue(ui->spnTreeLoadDistance, conf->getValue("fTreeLoadDistance"), 7800.0f, ui->sldTreeLoadDistance, 1000);

    conf->endSection();
    conf->startSection("Grass");

    setDefaultValue(ui->spnGrassDrawDistance, conf->getValue("fGrassStartFadeDistance"), 5000.0f, ui->sldGrassDrawDistance, 1000);
    setDefaultValue(ui->ckbEnhancedGrass, conf->getValue("bGrassPointLighting"), 0);
    setDefaultValue(ui->spnGrassDensi, conf->getValue("iMinGrassSize"), 20, ui->sldGrassDensi, 1);

    conf->endSection();
    conf->startSection("Water");

    setDefaultValue(ui->grpReflect, conf->getValue("bUseWaterReflections"), 1);
    setDefaultValue(ui->ckbForceHD, conf->getValue("bForceHighDetailReflections"), 0);
    setDefaultValue(ui->ckbReflectBlur, conf->getValue("bUseWaterReflectionBlur"), 0);
    setDefaultValue(ui->ckbReflect_Explosions, conf->getValue("bReflectExplosions"), 0);
    setDefaultValue(ui->ckbReflect_Land, conf->getValue("bReflectLODLand"), 1);
    setDefaultValue(ui->ckbReflect_Objects, conf->getValue("bReflectLODObjects"), 1);
    setDefaultValue(ui->ckbReflect_Sky, conf->getValue("bReflectSky"), 1);
    setDefaultValue(ui->ckbReflect_Trees, conf->getValue("bReflectLODTrees"), 1);
    setDefaultValue(ui->spnReflectBlur, conf->getValue("iWaterBlurAmount"), 0, ui->sldReflectBlur, 1);

    switch(conf->getValue("iWaterReflectHeight").toInt())
    {
    case 256:
        ui->cmbWaterRes->setCurrentIndex(0);
        break;
    case 512:
        ui->cmbWaterRes->setCurrentIndex(1);
        break;
    case 1024:
        ui->cmbWaterRes->setCurrentIndex(2);
        break;
    case 2048:
        ui->cmbWaterRes->setCurrentIndex(3);
        break;
    case 4096:
        ui->cmbWaterRes->setCurrentIndex(4);
        break;
    case 8192:
        ui->cmbWaterRes->setCurrentIndex(5);
        break;
    }

    setDefaultValue(ui->ckbRefract, conf->getValue("bUseWaterRefractions"), 1);
    setDefaultValue(ui->ckbDisplace, conf->getValue("bUseWaterDisplacements"), 1);
    setDefaultValue(ui->ckbDepth, conf->getValue("bUseWaterDepth"), 1);
    setDefaultValue(ui->ckbWaterShader, conf->getValue("bUseWaterShader"), 1);
    setDefaultValue(ui->ckbWaterLOD, conf->getValue("bUseWaterLOD"), 1);
    setDefaultValue(ui->ckbSilRefl, conf->getValue("bAutoWaterSilhouetteReflections"), 1);

    conf->endSection();
    conf->startSection("Controls");

    setDefaultValue(ui->ckbMouseAccel, conf->getValue("bMouseAcceleration"), 1);
    setDefaultValue(ui->ckbMouseInvertY, conf->getValue("bInvertYValues"), 0);
    setDefaultValue(ui->spnMouseSens, conf->getValue("fMouseHeadingSensitivity"), 0.02f);
    setDefaultValue(ui->spnMouseSCalX, conf->getValue("fMouseHeadingXScale"), 0.0200);
    setDefaultValue(ui->spnMouseSCalY, conf->getValue("fMouseHeadingYScale"), 0.8500);

    conf->endSection();
    conf->startSection("GamePlay");

    setDefaultValue(ui->ckbQuestMarkers, conf->getValue("bShowQuestMarkers"), 1);
    setDefaultValue(ui->ckbQuestMarkersFloat, conf->getValue("bShowFloatingQuestMarkers"), 1);

    conf->endSection();
    conf->startSection("Interface");

    setDefaultValue(ui->ckbCompass, conf->getValue("bShowCompass"), 1);
    setDefaultValue(ui->ckbSubtitles, conf->getValue("bGeneralSubtitles"), 0);
    setDefaultValue(ui->ckbSubtitlesDialogue, conf->getValue("bDialogueSubtitles"), 0);

    conf->endSection();
    conf->startSection("MAIN");

    setDefaultValue(ui->ckbSavePause, conf->getValue("bSaveOnPause"), 1);
    setDefaultValue(ui->ckbSaveRest, conf->getValue("bSaveOnRest"), 1);
    setDefaultValue(ui->ckbSaveTravel, conf->getValue("bSaveOnTravel"), 1);
    setDefaultValue(ui->ckbSaveWait, conf->getValue("bSaveOnWait"), 1);
    setDefaultValue(ui->spnHUDOpacity, conf->getValue("fHUDOpacity"), 1.0, ui->sldHUDOpacity);
    setDefaultValue(ui->spnSkyCellFadeDist, conf->getValue("fSkyCellRefFadeDistance"), 150000.0f, ui->sldSkyCellFadeDist, 1000);

    conf->endSection();
    conf->startSection("SaveGame");

    if(conf->getValue("bDisableAutoSave").toInt() == 0)
    {
        setDefaultValue(ui->grpAutoSave, "1", 1);
    }
    else
    {
        setDefaultValue(ui->grpAutoSave, "0", 1);
    }

    setDefaultValue(ui->sldAutoSaveInt, conf->getValue("fAutosaveEveryXMins"), 15, 1);

    conf->endSection();
    conf->startSection("Launcher");

    setDefaultValue(ui->ckbFileSelection, conf->getValue("bEnableFileSelection"), 1);

    conf->endSection();
    conf->startSection("TerrainManager");

    setDefaultValue(ui->spnBlockDistance, conf->getValue("fBlockLevel1Distance"), 40000.0f, ui->sldBlockDistance, 1000);
    setDefaultValue(ui->spnSplitDistanceMul, conf->getValue("fSplitDistanceMult"), 1.1f, ui->sldSplitDistanceMul);

    conf->endSection();
}

void MainWindow::setPreview(QString image1, QString image2)
{   
    if(ui->ckbShowPreview->isChecked())
    {
        skrmclog->event(LOGLV_NORMAL, "loading preview images \""+image1+"\", \""+image2+"\"");
        QPixmap map1(preview_base+"/previews/"+image1);
        QPixmap map2(preview_base+"/previews/"+image2);

        if(map1.isNull())
        {
            skrmclog->event(LOGLV_WARNING, "couldn't load \""+image1+"\"");
            map1.load(":/previews/000.jpg");
        }

        if(map2.isNull())
        {
            skrmclog->event(LOGLV_WARNING, "couldn't load \""+image2+"\"");
            map2.load(":/previews/000.jpg");
        }

        ui->viwPreview1->setPixmap(map1.scaledToWidth(400));
        ui->viwPreview2->setPixmap(map2.scaledToWidth(400));

        if(ui->ckbPrevResSlider->isChecked())
        {
            ui->sldPreview->setValue(200);
        }

        ui->sldPreview->setEnabled(true);
        ui->sldPreview->setVisible(true);
        ui->ckbPrevResSlider->setVisible(true);
    }
}

void MainWindow::PreviewHandler(QString element, int value)
{
    if(!ui->ckbShowPreview->isChecked())
        return;

    CPreview *tmp = pPreviews->getPreview(element, value);

    if(tmp != NULL)
    {
        if(tmp->isSplit())
        {
            setPreview(tmp->getImgA(), tmp->getImgB());
        }
        else
        {
            setPreview(tmp->getImgA());
        }
    }
}

void MainWindow::setPreview(QString image1)
{
    if(ui->ckbShowPreview->isChecked())
    {
        skrmclog->event(LOGLV_NORMAL, "loading preview image \""+image1+"\"");
        QPixmap map1(preview_base+"/previews/"+image1);

        if(map1.isNull())
        {
            skrmclog->event(LOGLV_WARNING, "couldn't load \""+image1+"\"");
            map1.load(":/previews/000.jpg");
        }

        ui->viwPreview1->setPixmap(map1.scaledToWidth(400));
        ui->sldPreview->setValue(400);
        ui->sldPreview->setDisabled(true);
        ui->sldPreview->setVisible(false);
        ui->ckbPrevResSlider->setVisible(false);
    }
}

void MainWindow::on_sldTexFilter_valueChanged(int value)
{
    switch(value)
    {
    case 0:
        ui->lblTexFilterInd->setText("off");
        break;
    case 1:
        ui->lblTexFilterInd->setText("2x");
        break;
    case 2:
        ui->lblTexFilterInd->setText("4x");
        break;
    case 3:
        ui->lblTexFilterInd->setText("8x");
        break;
    case 4:
        ui->lblTexFilterInd->setText("16x");
        break;
    }
}

void MainWindow::on_spnGamma_valueChanged(double arg1)
{
    ui->sldGamma->setValue((int)(arg1 * 1000));
}

void MainWindow::on_sldDecalDrawDistance_valueChanged(int value)
{
    ui->spnDecalDrawDistance->setValue(((double)value / 1000.f));
}

void MainWindow::on_spnDecalDrawDistance_valueChanged(double arg1)
{
    ui->sldDecalDrawDistance->setValue((int)(arg1 * 1000));
}

void MainWindow::on_sldShadowDistInt_valueChanged(int value)
{
    ui->spnShadowDistInt->setValue((double)value / 1000.f);
}

void MainWindow::on_spnShadowDistInt_valueChanged(double arg1)
{
    ui->sldShadowDistInt->setValue((int)(arg1 * 1000));
}

void MainWindow::on_sldShadowDistExt_valueChanged(int value)
{
    ui->spnShadowDistExt->setValue((double)value / 1000.f);
}

void MainWindow::on_spnShadowDistExt_valueChanged(double arg1)
{
    ui->sldShadowDistExt->setValue((int)(arg1 * 1000));
}

void MainWindow::on_sldMouseSens_valueChanged(int value)
{
    ui->spnMouseSens->setValue((double)value / 1000);
}

void MainWindow::on_spnMouseSens_valueChanged(double arg1)
{
    ui->sldMouseSens->setValue((int)(arg1 * 1000));
}

void MainWindow::on_sldShadowBias_valueChanged(int value)
{
    ui->spnShadowBias->setValue((double)value / 10000);
}

void MainWindow::on_spnShadowBias_valueChanged(double arg1)
{
    ui->sldShadowBias->setValue((int)(arg1 * 10000));
}

void MainWindow::on_sldShadowDistFade_valueChanged(int value)
{
    ui->spnShadowDistFade->setValue((double)value / 1000);
}

void MainWindow::on_spnShadowDistFade_valueChanged(double arg1)
{
    ui->sldShadowDistFade->setValue((int)(arg1 * 1000));
}

void MainWindow::on_sldLightFadeDist_valueChanged(int value)
{
    ui->spnLightFadeDist->setValue((double)value / 1000);
}

void MainWindow::on_spnLightFadeDist_valueChanged(double arg1)
{
    ui->sldLightFadeDist->setValue((int)(arg1 * 1000));
}

void MainWindow::on_sldSpecFadeDist_valueChanged(int value)
{
    ui->spnSpecFadeDist->setValue((double)value / 1000);
}

void MainWindow::on_spnSpecFadeDist_valueChanged(double arg1)
{
    ui->sldSpecFadeDist->setValue((int)(arg1 * 1000));
}

void MainWindow::on_spnShadowMaskQuarter_valueChanged(int arg1)
{
    ui->sldShadowMaskQuarter->setValue(arg1);
}

void MainWindow::on_sldShadowMaskQuarter_valueChanged(int value)
{
    ui->spnShadowMaskQuarter->setValue(value);
}

void MainWindow::on_sldGrassDrawDistance_valueChanged(int value)
{
    ui->spnGrassDrawDistance->setValue((double)value / 1000.f);
}

void MainWindow::on_spnGrassDrawDistance_valueChanged(double arg1)
{
    ui->sldGrassDrawDistance->setValue((int)(arg1 * 1000.f));
}

void MainWindow::on_sldTreeDrawDistance_valueChanged(int value)
{
    ui->spnTreeDrawDistance->setValue((double)value / 1000.f);
}

void MainWindow::on_spnTreeDrawDistance_valueChanged(double arg1)
{
    ui->sldTreeDrawDistance->setValue((int)(arg1 * 1000.f));
}

void MainWindow::on_sldModelDrawDistance_valueChanged(int value)
{
    ui->spnModelDrawDistance->setValue((double)value / 100.f);
}

void MainWindow::on_spnModelDrawDistance_valueChanged(double arg1)
{
    ui->sldModelDrawDistance->setValue((int)(arg1 * 100.f));
}

void MainWindow::on_btnReset_clicked()
{
    delete conf;

    backup_window = new backups(this);
    backup_window->show();
    buwin_open = true;

    connect(backup_window, SIGNAL(reloadConfig()), this, SLOT(reloadConfig()));
}

void MainWindow::reloadConfig()
{
    skrmclog->event(LOGLV_NORMAL, "reloading configs");
    buwin_open = false;
    conf = new CSkyrimConfig();
    setDefaultValues();
}

void MainWindow::on_sldWaterMS_valueChanged(int value)
{
    switch(value)
    {
    case 0:
        ui->lblWaterMS_ind->setText("0x");
        break;
    case 1:
        ui->lblWaterMS_ind->setText("2x");
        break;
    case 2:
        ui->lblWaterMS_ind->setText("4x");
        break;
    case 3:
        ui->lblWaterMS_ind->setText("8x");
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(buwin_open)
    {
        event->ignore();
    }
    skrmclog->event(LOGLV_NORMAL, "exiting program");
}

void MainWindow::createActions()
{
    actSave = new QAction(tr("Save"), this);
    actSave->setShortcut(QKeySequence::Save);
    connect(actSave, SIGNAL(triggered()), this, SLOT(on_btnApply_clicked()));

    actReset = new QAction(tr("Load Backup"), this);
    actReset->setShortcut(QKeySequence("Ctrl+B"));
    connect(actReset, SIGNAL(triggered()), this, SLOT(on_btnReset_clicked()));

    actReload = new QAction(tr("Reload config"), this);
    actReload->setShortcut(QKeySequence("Ctrl+R"));
    connect(actReload, SIGNAL(triggered()), this, SLOT(on_btnReload_clicked()));

    actImport = new QAction(tr("Import config"), this);
    actImport->setShortcut(QKeySequence("Ctrl+I"));
    connect(actImport, SIGNAL(triggered()), this, SLOT(configImport()));

    actExport = new QAction(tr("Export config"), this);
    actExport->setShortcut(QKeySequence("Ctrl+E"));
    connect(actExport, SIGNAL(triggered()), this, SLOT(configExport()));

    actExit = new QAction(tr("Close"), this);
    actExit->setShortcut(QKeySequence("Ctrl+Q"));
    connect(actExit, SIGNAL(triggered()), this, SLOT(close()));

    actAbout = new QAction(tr("About"), this);
    actAbout->setShortcut(QKeySequence("Ctrl+A"));
    connect(actAbout, SIGNAL(triggered()), this, SLOT(openAbout()));

}

void MainWindow::createMenus()
{
    menFile = menuBar()->addMenu(tr("File"));
    menFile->addAction(actSave);
    menFile->addAction(actReset);
    menFile->addAction(actReload);
    menFile->addSeparator();
    menFile->addAction(actImport);
    menFile->addAction(actExport);
    menFile->addSeparator();
    menFile->addAction(actExit);

    menuBar()->addAction(actAbout);
}

void MainWindow::openAbout()
{
    about a(this);
    a.exec();
    this->setEnabled(true);
}

void MainWindow::PreviewHandlerInt(int v)
{
    QString element = sender()->objectName();
    PreviewHandler(element, v);
}

void MainWindow::PreviewHandlerBool(bool v)
{
    QString element = sender()->objectName();
    int val = v ? 1 : 0;
    PreviewHandler(element, val);
}

void MainWindow::PreviewHandlerDblH(double v)
{
    QString element = sender()->objectName();
    int val = static_cast<int>(v);
    PreviewHandler(element, val);
}

void MainWindow::PreviewHandlerDblL(double v)
{
    QString element = sender()->objectName();
    int val = static_cast<int>(v * 1000);
    PreviewHandler(element, val);
}

void MainWindow::configImport()
{
    QMessageBox msg;
    QFileDialog file;
    //file.setReadOnly(true);
    //file.setFilter("*.xml");
    //file.exec();

    QString filename = file.getOpenFileName(this, "Import file", "", "*.xml");

    if(filename.isEmpty())
    {
        return;
    }

    skrmclog->event(LOGLV_NORMAL, "got \""+filename+"\" for import");

    importer *im = new importer(this, filename);
    im->open();
    connect(im, SIGNAL(doImport(QString)), SLOT(configDoImport(QString)));

/*

    */
}

void MainWindow::configExport()
{
    QFileDialog file;

    QString filename = file.getSaveFileName(this, "Export to...", "", "*.xml");
    //QString filename = file.selectedFiles().at(0);

    if(filename.isEmpty())
        return;

    if(!filename.endsWith(".xml"))
        filename.append(".xml");

    skrmclog->event(LOGLV_NORMAL, "got \""+filename+"\" for export");

    exporter* window = new exporter(this, base_path, filename);
    window->show();
}

void MainWindow::configDoImport(QString filename)
{
    QMessageBox msg;
    unsigned short int uFile = SKYRIM_USER;
    QString key;

    QFile *pFile = new QFile(filename);
    if(!pFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        skrmclog->event(LOGLV_ERROR, "couldn't open \""+filename+"\" for reading!");
        msg.setText(tr("couldn't import file!"));
        msg.setWindowTitle(tr("Error!"));
        msg.setIcon(QMessageBox::Warning);
        msg.exec();
        return;
    }
    QXmlStreamReader *pReader = new QXmlStreamReader(pFile);

    while(pReader->readNextStartElement())
    {
        if(pReader->name() == "cfg")
        {
            if(pReader->attributes().value("ver") == "1.0")
            {
                continue;
            }
            else
            {
                msg.setText(tr("couldn't import file!"));
                msg.setWindowTitle(tr("Error!"));
                msg.setIcon(QMessageBox::Warning);
                msg.exec();
                return;
            }
        }
        else if(pReader->name() == "config")
        {
            if(pReader->attributes().value("file") == "user")
            {
                uFile = SKYRIM_USER;
            }
            else if(pReader->attributes().value("file") == "main")
            {
                uFile = SKYRIM_MAIN;
            }

            while(pReader->readNextStartElement())
            {
                if(pReader->name() == "section")
                {
                    conf->startSection(pReader->attributes().value("name").toString());

                    while(pReader->readNextStartElement())
                    {
                        if(pReader->name() == "value")
                        {
                            if((key = pReader->attributes().value("key").toString()).isEmpty())
                                continue;

                            conf->setValue(key, pReader->readElementText(), uFile);
                        }
                        else pReader->skipCurrentElement();
                    }

                    conf->endSection();
                }
                else pReader->skipCurrentElement();
            }
        }
        else pReader->skipCurrentElement();
    }

    pReader->clear();
    pFile->close();

    conf->save();
    delete conf;

    reloadConfig();

    disconnect(this, SLOT(configDoImport(QString)));

    msg.setText(tr("Successfully imported the config!"));
    msg.exec();

    skrmclog->event(LOGLV_NORMAL, "successfully imported file!");
}

void MainWindow::on_cmbShadowRes_currentIndexChanged(int index)
{
    static bool bFirstWarn = true;
    if(index == 5)
    {
        if(bFirstWarn)
        {
            QMessageBox msg;
            msg.setText(tr("Warning: High Shadow resolutions heavily impact performance!"));
            msg.setIcon(QMessageBox::Warning);
            msg.setWindowTitle(tr("Warning!"));
            msg.exec();
            bFirstWarn = false;
        }
    }
}

void MainWindow::on_cmbWaterRes_currentIndexChanged(int index)
{
    static bool bFirstWarn = true;
    if(index == 5)
    {
        if(bFirstWarn)
        {
            QMessageBox msg;
            msg.setText(tr("Warning: High Water reflection resolutions heavily impact performance and can also lead to crashes if there's not enough VRAM!"));
            msg.setIcon(QMessageBox::Warning);
            msg.setWindowTitle(tr("Warning!"));
            msg.exec();
            bFirstWarn = false;
        }
    }
}

void MainWindow::on_sldSunUpdate_valueChanged(int value)
{
    ui->spnSunUpdate->setValue((double)value/10000.f);
}

void MainWindow::on_spnSunUpdate_valueChanged(double arg1)
{
    ui->sldSunUpdate->setValue((int)(arg1*10000));
}

void MainWindow::on_sldSunThres_valueChanged(int value)
{
    ui->spnSunThres->setValue((double)value/10000.f);
}

void MainWindow::on_spnSunThres_valueChanged(double arg1)
{
    ui->sldSunThres->setValue((int)(arg1*10000));
}

void MainWindow::on_sldTreeLoadDistance_valueChanged(int value)
{
    ui->spnTreeLoadDistance->setValue((double)value/1000.f);
}

void MainWindow::on_spnTreeLoadDistance_valueChanged(double arg1)
{
    ui->sldTreeLoadDistance->setValue((double)(arg1*1000));
}

void MainWindow::on_sldGrids_valueChanged(int value)
{
    static bool bFirstWarn1 = true;
    QMessageBox msg;

    ui->spnGrids->setValue((value + (value - 1)));

    if(value == ui->sldGrids->maximum())
    {
        if(bFirstWarn1)
        {
            msg.setText(tr("Warning: loading 13 Grids is very unstable!"));
            msg.setIcon(QMessageBox::Warning);
            msg.setWindowTitle(tr("Warning!"));
            msg.exec();
            bFirstWarn1 = false;
        }
    }
}

void MainWindow::on_spnGrids_valueChanged(int arg1)
{
    ui->sldGrids->setValue(((arg1 + 1)/2));
}

void MainWindow::on_sldPreview_valueChanged(int value)
{
    ui->viwPreview1->setFixedWidth(value);
    ui->viwPreview2->setFixedWidth(400-value);
    //ui->viwPreview2->setGeometry(210-(200-value), 20, 200+(200-value), 225);
}

void MainWindow::on_btnReload_clicked()
{
    skrmclog->event(LOGLV_NORMAL, "reloading configs");
    delete conf;
    conf = new CSkyrimConfig();
    setDefaultValues();
}

void MainWindow::on_sldFOV_valueChanged(int value)
{
    ui->lblFOV->setToolTip(tr("changes Field of view.\n after changing it open the console in game and type \"fov ")+QVariant(value).toString()+tr("\" to apply it to your savegame. "));
    ui->sldFOV->setToolTip(tr("changes Field of view.\n after changing it open the console in game and type \"fov ")+QVariant(value).toString()+tr("\" to apply it to your savegame. "));
    ui->spnFOV->setToolTip(tr("changes Field of view.\n after changing it open the console in game and type \"fov ")+QVariant(value).toString()+tr("\" to apply it to your savegame. "));
}

void MainWindow::on_ckbReflect_Objects_toggled(bool checked)
{
    Q_UNUSED(checked)
    if(ui->ckbReflect_Land->isChecked() && ui->ckbReflect_Objects->isChecked() && ui->ckbReflect_Sky->isChecked() && ui->ckbReflect_Trees->isChecked())
    {
        setPreview("water/water_reflect_no.jpg", "water/water_reflect_all.jpg");
    }
    else
    {
        setPreview("water/water_reflect_no.jpg", "water/water_reflect_obj.jpg");
    }
}

void MainWindow::on_ckbReflect_Trees_toggled(bool checked)
{
    Q_UNUSED(checked)
    if(ui->ckbReflect_Land->isChecked() && ui->ckbReflect_Objects->isChecked() && ui->ckbReflect_Sky->isChecked() && ui->ckbReflect_Trees->isChecked())
    {
        setPreview("water/water_reflect_no.jpg", "water/water_reflect_all.jpg");
    }
    else
    {
        setPreview("water/water_reflect_no.jpg", "water/water_reflect_tree.jpg");
    }
}

void MainWindow::on_ckbReflect_Land_toggled(bool checked)
{
    Q_UNUSED(checked)
    if(ui->ckbReflect_Land->isChecked() && ui->ckbReflect_Objects->isChecked() && ui->ckbReflect_Sky->isChecked() && ui->ckbReflect_Trees->isChecked())
    {
        setPreview("water/water_reflect_no.jpg", "water/water_reflect_all.jpg");
    }
    else
    {
        setPreview("water/water_reflect_no.jpg", "water/water_reflect_land.jpg");
    }
}

void MainWindow::on_ckbReflect_Sky_toggled(bool checked)
{
    Q_UNUSED(checked)
    if(ui->ckbReflect_Land->isChecked() && ui->ckbReflect_Objects->isChecked() && ui->ckbReflect_Sky->isChecked() && ui->ckbReflect_Trees->isChecked())
    {
        setPreview("water/water_reflect_no.jpg", "water/water_reflect_all.jpg");
    }
    else
    {
        setPreview("water/water_reflect_no.jpg", "water/water_reflect_sky.jpg");
    }
}

void MainWindow::on_cmbShadowQuality_simple_currentIndexChanged(int index)
{
    bool bOld = ui->ckbShowPreview->isChecked();
    ui->ckbShowPreview->setChecked(false);
    switch(index)
    {
    case 0:
        ui->cmbShadowFilter->setCurrentIndex(0);
        ui->cmbShadowRes->setCurrentIndex(0);
        ui->ckbTreeShadows->setChecked(false);
        ui->ckbLandShadows->setChecked(false);
        ui->ckbTorchShadow->setChecked(false);
        ui->ckbPlrSelfShad->setChecked(false);
        ui->spnShadowBias->setValue(0.75);
        ui->spnShadowBlur->setValue(5);
        ui->spnShadowDistExt->setValue(1500.0);
        ui->spnShadowDistInt->setValue(2000.0);
        ui->spnShadowMaskQuarter->setValue(1);
        ui->ckbGrassShadows->setChecked(false);
        ui->ckbShadowLOD->setChecked(true);
        break;
    case 1:
        ui->cmbShadowFilter->setCurrentIndex(1);
        ui->cmbShadowRes->setCurrentIndex(1);
        ui->ckbTreeShadows->setChecked(false);
        ui->ckbLandShadows->setChecked(false);
        ui->ckbTorchShadow->setChecked(false);
        ui->ckbPlrSelfShad->setChecked(false);
        ui->spnShadowBias->setValue(0.6);
        ui->spnShadowBlur->setValue(3);
        ui->spnShadowDistExt->setValue(2000.0);
        ui->spnShadowDistInt->setValue(4000.0);
        ui->spnShadowMaskQuarter->setValue(2);
        ui->ckbGrassShadows->setChecked(false);
        ui->ckbShadowLOD->setChecked(true);
        break;
    case 2:
        ui->cmbShadowFilter->setCurrentIndex(2);
        ui->cmbShadowRes->setCurrentIndex(2);
        ui->ckbTreeShadows->setChecked(true);
        ui->ckbLandShadows->setChecked(true);
        ui->ckbTorchShadow->setChecked(false);
        ui->ckbPlrSelfShad->setChecked(false);
        ui->spnShadowBias->setValue(0.5);
        ui->spnShadowBlur->setValue(1);
        ui->spnShadowDistExt->setValue(3500.0);
        ui->spnShadowDistInt->setValue(4000.0);
        ui->spnShadowMaskQuarter->setValue(3);
        ui->ckbGrassShadows->setChecked(false);
        ui->ckbShadowLOD->setChecked(true);
        break;
    case 3:
        ui->cmbShadowFilter->setCurrentIndex(3);
        ui->cmbShadowRes->setCurrentIndex(3);
        ui->ckbTreeShadows->setChecked(true);
        ui->ckbLandShadows->setChecked(true);
        ui->ckbTorchShadow->setChecked(true);
        ui->ckbPlrSelfShad->setChecked(true);
        ui->spnShadowBias->setValue(0.5);
        ui->spnShadowBlur->setValue(1);
        ui->spnShadowDistExt->setValue(4000.0);
        ui->spnShadowDistInt->setValue(4000.0);
        ui->spnShadowMaskQuarter->setValue(5);
        ui->ckbGrassShadows->setChecked(true);
        ui->ckbShadowLOD->setChecked(true);
        break;
    case 4:
        ui->cmbShadowFilter->setCurrentIndex(3);
        ui->cmbShadowRes->setCurrentIndex(4);
        ui->ckbTreeShadows->setChecked(true);
        ui->ckbLandShadows->setChecked(true);
        ui->ckbTorchShadow->setChecked(true);
        ui->ckbPlrSelfShad->setChecked(true);
        ui->spnShadowBias->setValue(0.35);
        ui->spnShadowBlur->setValue(1);
        ui->spnShadowDistExt->setValue(6000.0);
        ui->spnShadowDistInt->setValue(4000.0);
        ui->spnShadowMaskQuarter->setValue(8);
        ui->ckbGrassShadows->setChecked(true);
        ui->ckbShadowLOD->setChecked(false);
        break;
    case 5:
        ui->cmbShadowFilter->setCurrentIndex(3);
        ui->cmbShadowRes->setCurrentIndex(5);
        ui->ckbTreeShadows->setChecked(true);
        ui->ckbLandShadows->setChecked(true);
        ui->ckbTorchShadow->setChecked(true);
        ui->ckbPlrSelfShad->setChecked(true);
        ui->spnShadowBias->setValue(0.3);
        ui->spnShadowBlur->setValue(1);
        ui->spnShadowDistExt->setValue(7000.0);
        ui->spnShadowDistInt->setValue(4000.0);
        ui->spnShadowMaskQuarter->setValue(10);
        ui->ckbGrassShadows->setChecked(true);
        ui->ckbShadowLOD->setChecked(false);
        break;
    case 6:
        break;
    }
    ui->ckbShowPreview->setChecked(bOld);
}

void MainWindow::setDefaultValue(QSlider *sld, QString value, int def, int conv)
{
    int val;
    if(value != "NULL")
    {
        val = (int)value.toDouble();
        val = val * conv;

        if(value == "10000000.0000")
        {
            sld->setValue(sld->maximum());
            return;
        }
        else if(val > sld->maximum())
        {
            sld->setMaximum(val);
        }

        sld->setValue(val);
    }
    else
    {
        sld->setValue(def);
    }
}

void MainWindow::setDefaultValue(QSpinBox *spn, QString value, int def, QSlider *sld, int conv)
{
    int val;
    if(value != "NULL")
    {
        val = (int)value.toDouble();
        val = val * conv;

        if(value == "10000000.0000")
        {
            spn->setValue(spn->maximum());
            return;
        }
        else if(val > spn->maximum())
        {
            spn->setMaximum(val);
            sld->setMaximum(val);
        }

        spn->setValue(val);
    }
    else
    {
        spn->setValue(def);
    }
}

void MainWindow::setDefaultValue(QDoubleSpinBox *spn, QString value, double def)
{
    double val;
    if(value != "NULL")
    {
        val = value.toDouble();
        if(value == "10000000.0000")
        {
            spn->setValue(spn->maximum());
            return;
        }
        else if(val > spn->maximum())
        {
            spn->setMaximum(val);
        }
        spn->setValue(val);
    }
    else
    {
        spn->setValue(def);
    }
}

void MainWindow::setDefaultValue(QSpinBox *spn, QString value, int def, int conv)
{
    int val;
    if(value != "NULL")
    {
        val = value.toInt();
        val = val * conv;

        if(value == "10000000.0000")
        {
            spn->setValue(spn->maximum());
            return;
        }
        else if(val > spn->maximum())
        {
            spn->setMaximum(val);
        }

        spn->setValue(val);
    }
    else
    {
        spn->setValue(def);
    }
}

void MainWindow::setDefaultValue(QDoubleSpinBox *spn, QString value, double def, QSlider *sld, int conv)
{
    double val;
    if(value != "NULL")
    {
        val = value.toDouble();
        if(value == "10000000.0000")
        {
            spn->setValue(spn->maximum());
            return;
        }
        else if(val > spn->maximum())
        {
            spn->setMaximum(val);
            sld->setMaximum(static_cast<int>(val * conv));
        }
        spn->setValue(val);
    }
    else
    {
        spn->setValue(def);
    }
}

void MainWindow::setDefaultValue(QLineEdit *txt, QString value, QString def)
{
    if(value != "NULL")
    {
        txt->setText(value);
    }
    else
    {
        txt->setText(def);
    }
}

void MainWindow::setDefaultValue(QComboBox *cmb, QString value, int def)
{
    if(value != "NULL")
    {
        cmb->setCurrentIndex(value.toInt());
    }
    else
    {
        cmb->setCurrentIndex(def);
    }
}

void MainWindow::setDefaultValue(QCheckBox *ckb, QString value, int def)
{
    if(value != "NULL")
    {
        ckb->setChecked(value.toInt());
    }
    else
    {
        ckb->setChecked(def);
    }
}

void MainWindow::setDefaultValue(QGroupBox *ckb, QString value, int def)
{
    if(value != "NULL")
    {
        ckb->setChecked(value.toInt());
    }
    else
    {
        ckb->setChecked(def);
    }
}

bool bFirst = true;

void MainWindow::on_sldMouseSCalX_valueChanged(int value)
{
    static int old = value;
    if(ui->radPropMScaling->isChecked() && bFirst)
    {
        bFirst = false;
        ui->sldMouseSCalY->setValue(ui->sldMouseSCalY->value() + (value-old));
        bFirst = true;
    }
    else if(ui->radInvPropMScaling->isChecked() && bFirst)
    {
        bFirst = false;
        ui->sldMouseSCalY->setValue(ui->sldMouseSCalY->value() - (value-old));
        bFirst = true;
    }
    else if(ui->radSyncMScaling->isChecked())
    {
        ui->sldMouseSCalY->setValue(value);
    }

    old = value;

    ui->spnMouseSCalX->setValue((double)value/10000.0f);
}

void MainWindow::on_spnMouseSCalX_valueChanged(double arg1)
{
    ui->sldMouseSCalX->setValue(static_cast<int>(arg1*10000));
}

void MainWindow::on_sldMouseSCalY_valueChanged(int value)
{
    static int old = value;
    if(ui->radPropMScaling->isChecked() && bFirst)
    {
        bFirst = false;
        ui->sldMouseSCalX->setValue(ui->sldMouseSCalX->value() + (value-old));
        bFirst = true;
    }
    else if(ui->radInvPropMScaling->isChecked() && bFirst)
    {
        bFirst = false;
        ui->sldMouseSCalX->setValue(ui->sldMouseSCalX->value() - (value-old));
        bFirst = true;
    }
    else if(ui->radSyncMScaling->isChecked())
    {
        ui->sldMouseSCalX->setValue(value);
    }

    old = value;

    ui->spnMouseSCalY->setValue((double)value/10000.0f);
}

void MainWindow::on_spnMouseSCalY_valueChanged(double arg1)
{
    ui->sldMouseSCalY->setValue(static_cast<int>(arg1*10000));
}

void MainWindow::on_sldHUDOpacity_valueChanged(int value)
{
    ui->spnHUDOpacity->setValue((double)value / 10000.f);
}

void MainWindow::on_spnHUDOpacity_valueChanged(double arg1)
{
    ui->sldHUDOpacity->setValue(static_cast<int>(arg1 * 10000));
}

void MainWindow::on_sldBlockDistance_valueChanged(int value)
{
    ui->spnBlockDistance->setValue((double)value / 1000.f);
}

void MainWindow::on_spnBlockDistance_valueChanged(double arg1)
{
    ui->sldBlockDistance->setValue(static_cast<int>(arg1 * 1000));
}

void MainWindow::on_sldSplitDistanceMul_valueChanged(int value)
{
    ui->spnSplitDistanceMul->setValue((double)value / 10000.f);
}

void MainWindow::on_spnSplitDistanceMul_valueChanged(double arg1)
{
    ui->sldSplitDistanceMul->setValue(static_cast<int>(arg1 * 10000));
}

void MainWindow::on_sldSkyCellFadeDist_valueChanged(int value)
{
    ui->spnSkyCellFadeDist->setValue((double)value / 1000.f);
}

void MainWindow::on_spnSkyCellFadeDist_valueChanged(double arg1)
{
    ui->sldSkyCellFadeDist->setValue(static_cast<int>(arg1 * 1000));
}

void MainWindow::on_sldNearDistance_valueChanged(int value)
{
    ui->spnNearDistance->setValue((double)value / 10000.f);
}

void MainWindow::on_spnNearDistance_valueChanged(double arg1)
{
    ui->sldNearDistance->setValue(static_cast<int>(arg1 * 10000));
}

void MainWindow::spnGamma_change()
{
    if(init) return;
	conf->startSection("Display");
	conf->setValue("fGamma", ui->spnGamma->value(), SKYRIM_MAIN);
}
void MainWindow::ckbFPRT_change()
{
    if(init) return;
	conf->startSection("Display");
	conf->setValue("bFloatPointRenderTarget", ui->ckbFPRT->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbScreenFuScr_change()
{
    if(init) return;
	conf->startSection("Display");
    conf->setValue("bFull Screen", ui->ckbScreenFuScr->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbVSync_change()
{
    if(init) return;
	conf->startSection("Display");
	conf->setValue("iPresentInterval", ui->ckbVSync->isChecked(), SKYRIM_MAIN);
	conf->setValue("iPresentInterval", ui->ckbVSync->isChecked(), SKYRIM_USER);
}
void MainWindow::txtScreenresH_change()
{
    if(init) return;
    conf->startSection("Display");
    conf->setValue("iSize H", ui->txtScreenResH->text(), SKYRIM_MAIN);
}
void MainWindow::txtScreenresW_change()
{
    if(init) return;
    conf->startSection("Display");
    conf->setValue("iSize W", ui->txtScreenResW->text(), SKYRIM_MAIN);
}
void MainWindow::spnFOV_change()
{
    if(init) return;
    conf->startSection("Display");
    conf->setValue("fDefaultWorldFOV", ui->sldFOV->value(), SKYRIM_USER);
    conf->setValue("fDefault1stPersonFOV", ui->sldFOV->value(), SKYRIM_USER);
    conf->startSection("General");
    conf->setValue("fDefaultFOV", ui->sldFOV->value(), SKYRIM_MAIN);
}
void MainWindow::ckbFXAA_change()
{
    if(init) return;
    conf->startSection("Display");
    conf->setValue("bFXAAEnabled", ui->ckbFXAA->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbTransAA()
{
    if(init) return;
    conf->startSection("Display");
    conf->setValue("bTransparencyMultisampling", ui->ckbTransAA->isChecked(), SKYRIM_MAIN);
}
void MainWindow::sldAA_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("iMultiSample", (double)pow(2.f, (float)ui->sldAA->value()), SKYRIM_MAIN);
}
void MainWindow::cmbRadBlurLev_change()
{
	if(init) return;
    conf->startSection("Imagespace");
    if(ui->cmbRadBlurLev->currentIndex() > 0)
    {
        conf->setValue("iRadialBlurLevel", (ui->cmbRadBlurLev->currentIndex() - 1), SKYRIM_MAIN);
        conf->setValue("bDoRadialBlur", 1, SKYRIM_MAIN);
    }
    else
    {
        conf->setValue("bDoRadialBlur", 0, SKYRIM_MAIN);
    }
}
void MainWindow::ckbDOF_change()
{
	if(init) return;
    conf->startSection("Imagespace");
    conf->setValue("bDoDepthOfField", ui->ckbDOF->isChecked(), SKYRIM_MAIN);
}
void MainWindow::spnHUDOpacity_change()
{
	if(init) return;
    conf->startSection("MAIN");
    conf->setValue("fHUDOpacity", ui->spnHUDOpacity->value(), SKYRIM_MAIN);
}
void MainWindow::ckbCompass_change()
{
	if(init) return;
    conf->startSection("Interface");
    conf->setValue("bShowCompass", ui->ckbCompass->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbQuestMarkers_change()
{
	if(init) return;
    conf->startSection("GamePlay");
    conf->setValue("bShowQuestMarkers", ui->ckbQuestMarkers->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbQuestMarkersFloat_change()
{
	if(init) return;
    conf->startSection("GamePlay");
    conf->setValue("bShowFloatingQuestMarkers", ui->ckbQuestMarkersFloat->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbSubtitles_change()
{
	if(init) return;
    conf->startSection("Interface");
    conf->setValue("bGeneralSubtitles", ui->ckbSubtitles->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbSubtitlesDialogue_change()
{
	if(init) return;
    conf->startSection("Interface");
    conf->setValue("bDialogueSubtitles", ui->ckbSubtitlesDialogue->isChecked(), SKYRIM_MAIN);
}
void MainWindow::spnMaxParticles_change()
{
	if(init) return;
    conf->startSection("Particles");
    conf->setValue("iMaxDesired", ui->sldMaxParticles->value(), SKYRIM_MAIN);
}
void MainWindow::ckbThreadedParticles_change()
{
	if(init) return;
    conf->startSection("General");
    conf->setValue("bUseThreadedParticleSystem", ui->ckbThreadedParticles->isChecked(), SKYRIM_MAIN);
}
void MainWindow::cmbTexQual_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("iTexMipMapSkip", 4 - ui->cmbTexQual->currentIndex(), SKYRIM_MAIN);
}
void MainWindow::sldTexFilter_change()
{
	if(init) return;
    conf->startSection("Display");
    switch(ui->sldTexFilter->value())
    {
    case 0:
        conf->setValue("iMaxAnisotropy", 0, SKYRIM_MAIN);
        break;
    default:
        conf->setValue("iMaxAnisotropy", pow(2.0f, (float)ui->sldTexFilter->value()), SKYRIM_MAIN);
        break;
    }
}
void MainWindow::ckbThreadedTex_change()
{
	if(init) return;
    conf->startSection("General");
    conf->setValue("bUseThreadedTextures", ui->ckbThreadedtex->isChecked(), SKYRIM_MAIN);
}
void MainWindow::spnMaxDecalsModelsPA_change()
{
	if(init) return;
    conf->startSection("Decals");
    conf->setValue("uMaxSkinDecalsPerActor", ui->sldMaxDecalsModelsPA->value(), SKYRIM_MAIN);
}
void MainWindow::spnMaxDecalsModels_change()
{
	if(init) return;
    conf->startSection("Decals");
    conf->setValue("uMaxSkinDecals", ui->sldMaxDecalsModels->value(), SKYRIM_MAIN);
}
void MainWindow::spnMaxDecals_change()
{
	if(init) return;
    conf->startSection("Decals");
    conf->setValue("uMaxDecals", ui->sldMaxDecals->value(), SKYRIM_MAIN);
}
void MainWindow::spnDecalDrawDistance_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("fDecalLOD1", ui->spnDecalDrawDistance->value(), SKYRIM_MAIN);
    conf->setValue("fDecalLOD2", ui->spnDecalDrawDistance->value() * 1.5, SKYRIM_MAIN);
}
void MainWindow::ckbDrawDecals_change()
{
	if(init) return;
    conf->startSection("Decals");
    conf->setValue("bDecals", ui->ckbDrawDecals->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbDrawDecalsmodels_change()
{
	if(init) return;
    conf->startSection("Decals");
    conf->setValue("bSkinnedDecals", ui->ckbDrawDecalsModels->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbThreadedDecals_change()
{
	if(init) return;
    conf->startSection("General");
    conf->setValue("bUseThreadedBlood", ui->ckbThreadedDecals->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbBorderRegion_change()
{
	if(init) return;
    conf->startSection("General");
    conf->setValue("bBorderRegionsEnabled", ui->ckbBorderRegion->isChecked(), SKYRIM_USER);
}
void MainWindow::ckbThreadedLOD_change()
{
	if(init) return;
    conf->startSection("General");
    conf->setValue("bUseThreadedLOD", ui->ckbThreadedLOD->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbThreadedMesh_change()
{
	if(init) return;
    conf->startSection("General");
    conf->setValue("bUseThreadedMeshes", ui->ckbThreadedMesh->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbThreadedMorpher_change()
{
	if(init) return;
    conf->startSection("General");
    conf->setValue("bUseThreadedMorpher", ui->ckbThreadedMorpher->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbUnloadOnFT_change()
{
	if(init) return;
    conf->startSection("BackgroundLoad");
    conf->setValue("bSelectivePurgeUnusedOnFastTravel", ui->ckbUnloadOnFT->isChecked(), SKYRIM_MAIN);
}
void MainWindow::spnItmDist_change()
{
	if(init) return;
    conf->startSection("LOD");
    conf->setValue("fLODFadeOutMultItems", ui->spnItmDist->value(), SKYRIM_MAIN);
}
void MainWindow::spnObjDist_change()
{
	if(init) return;
    conf->startSection("LOD");
    conf->setValue("fLODFadeOutMultObjects", ui->spnObjDist->value(), SKYRIM_MAIN);
}
void MainWindow::spnActDist_change()
{
	if(init) return;
    conf->startSection("LOD");
    conf->setValue("fLODFadeOutMultActors", ui->spnActDist->value(), SKYRIM_MAIN);
}
void MainWindow::spnModelDrawDistance_change()
{
	if(init) return;
    conf->startSection("Display");
    if(ui->spnModelDrawDistance->value() != ui->spnModelDrawDistance->maximum())
    {
        conf->setValue("fMeshLODLevel1FadeDist", ui->spnModelDrawDistance->value(), SKYRIM_MAIN);
        conf->setValue("fMeshLODLevel2FadeDist", ui->spnModelDrawDistance->value() * 0.75, SKYRIM_MAIN);
    }
    else
    {
        conf->setValue("fMeshLODLevel1FadeDist", QVariant("10000000.0000").toString(), SKYRIM_MAIN);
        conf->setValue("fMeshLODLevel2FadeDist", QVariant("10000000.0000").toString(), SKYRIM_MAIN);
    }
}
void MainWindow::spnGrids_change()
{
	if(init) return;
    conf->startSection("General");
    conf->setValue("uGridsToLoad", ui->spnGrids->value(), SKYRIM_USER);
    conf->setValue("uExterior Cell Buffer", pow((double)(ui->spnGrids->value() + 1), 2), SKYRIM_USER);
}
void MainWindow::spnNearDistance_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("fNearDistance", ui->spnNearDistance->value(), SKYRIM_USER);
}
void MainWindow::spnSplitDistanceMul_change()
{
	if(init) return;
    conf->startSection("TerrainManager");
    conf->setValue("fSplitDistanceMult", ui->spnSplitDistanceMul->value(), SKYRIM_MAIN);
}
void MainWindow::spnSkyCellFadeDist_change()
{
	if(init) return;
    conf->startSection("MAIN");
    conf->setValue("fSkyCellRefFadeDistance", ui->spnSkyCellFadeDist->value(), SKYRIM_MAIN);
}
void MainWindow::spnBlockDistance_change()
{
	if(init) return;
    conf->startSection("TerrainManager");
    conf->setValue("fBlockMaximumDistance", ui->spnBlockDistance->value() * 3.75, SKYRIM_MAIN);
    conf->setValue("fBlockLevel1Distance", ui->spnBlockDistance->value(), SKYRIM_MAIN);
    conf->setValue("fBlockLevel0Distance", ui->spnBlockDistance->value() * 0.625, SKYRIM_MAIN);
}
void MainWindow::spnAniTrees_change()
{
	if(init) return;
    conf->startSection("Trees");
    conf->setValue("uiMaxSkinnedTreesToRender", ui->spnAniTrees->value(), SKYRIM_MAIN);
}
void MainWindow::spnTreeLoadDistance_change()
{
	if(init) return;
    conf->startSection("Trees");
    conf->setValue("fTreeLoadDistance", ui->spnTreeLoadDistance->value(), SKYRIM_MAIN);
}
void MainWindow::ckbSkinnedTrees_change()
{
	if(init) return;
    conf->startSection("Trees");
    conf->setValue("bEnableTreeAnimations", ui->ckbSkinnedTrees->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbTreeThreads_change()
{
	if(init) return;
    conf->startSection("Trees");
    conf->setValue("bUseMultiThreadedTrees", ui->ckbTreeThreads->isChecked(), SKYRIM_MAIN);
}
void MainWindow::spnTreeDrawDistance_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("fMeshLODLevel1FadeTreeDistance", ui->spnTreeDrawDistance->value(), SKYRIM_MAIN);
    conf->setValue("fMeshLODLevel2FadeTreeDistance", ui->spnTreeDrawDistance->value() * 0.75, SKYRIM_MAIN);
    conf->setValue("fTreesMidLODSwitchDist", ui->spnTreeDrawDistance->value() * 0.5, SKYRIM_MAIN);

}
void MainWindow::spnGrassDensi_change()
{
	if(init) return;
    conf->startSection("Grass");
    conf->setValue("iMinGrassSize", ui->spnGrassDensi->value(), SKYRIM_USER);
}
void MainWindow::spnGrassDrawDistance_change()
{
	if(init) return;
    conf->startSection("Grass");
    conf->setValue("fGrassStartFadeDistance", ui->spnGrassDrawDistance->value(), SKYRIM_MAIN);
}
void MainWindow::ckbEnhancedGrass_change()
{
	if(init) return;
    conf->startSection("Grass");
    if(ui->ckbEnhancedGrass->isChecked())
    {
        conf->setValue("bGrassPointLighting", 1, SKYRIM_MAIN);
        conf->setValue("bDrawShaderGrass", 1, SKYRIM_MAIN);
    }
    else
    {
        conf->setValue("bGrassPointLighting", 0, SKYRIM_MAIN);
        conf->setValue("bDrawShaderGrass", 0, SKYRIM_MAIN);
    }
}
void MainWindow::spnShadowSplitC_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("iShadowSplitCount", ui->spnShadowSplitC->value(), SKYRIM_MAIN);
}
void MainWindow::cmbShadowMode_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("iShadowMode", ui->cmbShadowMode->currentIndex() + 1, SKYRIM_MAIN);
}
void MainWindow::spnShadowBlur_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("iBlurDeferredShadowMask", ui->sldShadowBlur->value(), SKYRIM_MAIN);
}
void MainWindow::cmbShadowRes_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("iShadowMapResolution", pow(2.0, 8 + ui->cmbShadowRes->currentIndex()), SKYRIM_MAIN);
    conf->setValue("iShadowMapResolutionPrimary", pow(2.0, 8 + ui->cmbShadowRes->currentIndex()), SKYRIM_MAIN);
    conf->setValue("iShadowMapResolutionPrimary", pow(2.0, 8 + ui->cmbShadowRes->currentIndex()), SKYRIM_USER);
    if(ui->ckbShadowLOD->isChecked())
    {
        conf->setValue("iShadowMapResolutionSecondary", (pow(2.0, 8 + ui->cmbShadowRes->currentIndex()) / 2), SKYRIM_MAIN);
    }
    else
    {
        conf->setValue("iShadowMapResolutionSecondary", (pow(2.0, 8 + ui->cmbShadowRes->currentIndex())), SKYRIM_MAIN);
    }
}
void MainWindow::spnShadowBias_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("fShadowBiasScale", ui->spnShadowBias->value(), SKYRIM_MAIN);
}
void MainWindow::cmbShadowFilter_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("iShadowFilter", ui->cmbShadowFilter->currentIndex() + 1, SKYRIM_MAIN);
}
void MainWindow::ckbDrawShadows_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("bDrawShadows", ui->ckbDrawShadows->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbPlrSelfShad_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("bActorSelfShadowing", ui->ckbPlrSelfShad->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbTreeShadows_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("bTreesReceiveShadows", ui->ckbTreeShadows->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbGrassShadows_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("bShadowsOnGrass", ui->ckbGrassShadows->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbLandShadows_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("bDrawLandShadows", ui->ckbLandShadows->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbTorchShadows_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("bEquippedTorchesCastShadows", ui->ckbTorchShadow->isChecked(), SKYRIM_MAIN);
}
void MainWindow::spnShadowDistExt_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("fShadowDistance", ui->spnShadowDistExt->value(), SKYRIM_MAIN);
}
void MainWindow::spnShadowDistInt_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("fInteriorShadowDistance", ui->spnShadowDistInt->value(), SKYRIM_MAIN);
}
void MainWindow::spnShadowMaskQuarter_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("iShadowMaskQuarter", ui->spnShadowMaskQuarter->value(), SKYRIM_MAIN);
}
void MainWindow::spnShadowDistFade_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("fShadowLODStartFade", ui->spnShadowDistFade->value(), SKYRIM_MAIN);
}
void MainWindow::spnSunUpdate_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("fSunShadowUpdateTime", ui->spnSunUpdate->value(), SKYRIM_USER);
}
void MainWindow::spnSpecFadeDist_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("fSpecularLODStartFade", ui->spnSpecFadeDist->value(), SKYRIM_MAIN);
}
void MainWindow::spnLightFadeDist_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("fLightLODStartFade", ui->spnLightFadeDist->value(), SKYRIM_MAIN);
}
void MainWindow::spnSunThres_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("fSunUpdateThreshold", ui->spnSunThres->value(), SKYRIM_USER);
}
void MainWindow::ckbSimpleLight_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("bSimpleLighting", ui->ckbSimpleLight->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbSunBeams_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("bUseSunbeams", ui->ckbSunBeams->isChecked(), SKYRIM_MAIN);
}
void MainWindow::sldWaterMS_change()
{
	if(init) return;
    switch(ui->sldWaterMS->value())
    {
    case 0:
        conf->setValue("iWaterMultiSamples", 0, SKYRIM_MAIN);
        break;
    case 1:
        conf->setValue("iWaterMultiSamples", 2, SKYRIM_MAIN);
        break;
    case 2:
        conf->setValue("iWaterMultiSamples", 4, SKYRIM_MAIN);
        break;
    case 3:
        conf->setValue("iWaterMultiSamples", 8, SKYRIM_MAIN);
        break;
    }
}
void MainWindow::ckbDepth_change()
{
	if(init) return;
    conf->startSection("Water");
    conf->setValue("bUseWaterDepth", ui->ckbDepth->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbDisplace_change()
{
	if(init) return;
    conf->startSection("Water");
    conf->setValue("bUseWaterDisplacements", ui->ckbDisplace->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbRefract_change()
{
	if(init) return;
    conf->startSection("Water");
    conf->setValue("bUseWaterRefractions", ui->ckbRefract->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbWaterLOD_change()
{
	if(init) return;
    conf->startSection("Water");
    conf->setValue("bUseWaterLOD", ui->ckbWaterLOD->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbWaterShader_change()
{
	if(init) return;
    conf->startSection("Water");
    conf->setValue("bUseWaterShader", ui->ckbWaterShader->isChecked(), SKYRIM_MAIN);
}
void MainWindow::spnReflectBlur_change()
{
	if(init) return;
    conf->startSection("Water");
    conf->setValue("iWaterBlurAmount", ui->sldReflectBlur->value(), SKYRIM_MAIN);
}
void MainWindow::cmbWaterRes_change()
{
	if(init) return;
    conf->startSection("Water");
    switch(ui->cmbWaterRes->currentIndex())
    {
    case 0:
        conf->setValue("iWaterReflectHeight", 256, SKYRIM_MAIN);
        conf->setValue("iWaterReflectWidth", 256, SKYRIM_MAIN);
        break;
    case 1:
        conf->setValue("iWaterReflectHeight", 512, SKYRIM_MAIN);
        conf->setValue("iWaterReflectWidth", 512, SKYRIM_MAIN);
        break;
    case 2:
        conf->setValue("iWaterReflectHeight", 1024, SKYRIM_MAIN);
        conf->setValue("iWaterReflectWidth", 1024, SKYRIM_MAIN);
        break;
    case 3:
        conf->setValue("iWaterReflectHeight", 2048, SKYRIM_MAIN);
        conf->setValue("iWaterReflectWidth", 2048, SKYRIM_MAIN);
        break;
    case 4:
        conf->setValue("iWaterReflectHeight", 4096, SKYRIM_MAIN);
        conf->setValue("iWaterReflectWidth", 4096, SKYRIM_MAIN);
        break;
    case 5:
        conf->setValue("iWaterReflectHeight", 8192, SKYRIM_MAIN);
        conf->setValue("iWaterReflectWidth", 8192, SKYRIM_MAIN);
        break;
    }
}
void MainWindow::ckbReflectExplosions_change()
{
	if(init) return;
    conf->startSection("Water");
    conf->setValue("bReflectExplosions", ui->ckbReflect_Explosions->isChecked(), SKYRIM_USER);
}
void MainWindow::ckbReflectSky_change()
{
	if(init) return;
    conf->startSection("Water");
    conf->setValue("bReflectSky", ui->ckbReflect_Sky->isChecked(), SKYRIM_USER);
}
void MainWindow::ckbSilRefl_change()
{
	if(init) return;
    conf->startSection("Water");
    conf->setValue("bAutoWaterSilhouetteReflections", ui->ckbSilRefl->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbReflectLand_change()
{
	if(init) return;
    conf->startSection("Water");
    conf->setValue("bReflectLODLand", ui->ckbReflect_Land->isChecked(), SKYRIM_USER);
}
void MainWindow::ckbReflectObjects_change()
{
	if(init) return;
    conf->startSection("Water");
    conf->setValue("bReflectLODObjects", ui->ckbReflect_Objects->isChecked(), SKYRIM_USER);
}
void MainWindow::ckbReflectTrees_change()
{
	if(init) return;
    conf->startSection("Water");
    conf->setValue("bReflectLODTrees", ui->ckbReflect_Trees->isChecked(), SKYRIM_USER);
}
void MainWindow::ckbForceHD_change()
{
	if(init) return;
    conf->startSection("Water");
    conf->setValue("bForceHighDetailReflections", ui->ckbForceHD->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbReflectBlur_change()
{
	if(init) return;
    conf->startSection("Water");
    conf->setValue("bUseWaterReflectionBlur", ui->ckbReflectBlur->isChecked(), SKYRIM_MAIN);
}
void MainWindow::grpReflect_change()
{
    if(init) return;
    conf->startSection("Water");
    conf->setValue("bUseWaterReflections", ui->grpReflect->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbMouseInvertY_change()
{
	if(init) return;
    conf->startSection("Controls");
    conf->setValue("bInvertYValues", ui->ckbMouseInvertY->isChecked(), SKYRIM_MAIN);
}
void MainWindow::spnMouseSCalY_change()
{
	if(init) return;
    conf->startSection("Controls");
    conf->setValue("fMouseHeadingYScale", ui->spnMouseSCalY->value(), SKYRIM_MAIN);
}
void MainWindow::spnMouseSCalX_change()
{
	if(init) return;
    conf->startSection("Controls");
    conf->setValue("fMouseHeadingXScale", ui->spnMouseSCalX->value(), SKYRIM_MAIN);
}
void MainWindow::spnMouseSens_change()
{
	if(init) return;
    conf->startSection("Controls");
    conf->setValue("fMouseHeadingSensitivity", ui->spnMouseSens->value(), SKYRIM_MAIN);
}
void MainWindow::ckbMouseAccel_change()
{
	if(init) return;
    conf->startSection("Controls");
    conf->setValue("bMouseAcceleration", ui->ckbMouseAccel->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbAlwaysActive_change()
{
	if(init) return;
    conf->startSection("General");
    conf->setValue("bAlwaysActive", ui->ckbAlwaysActive->isChecked(), SKYRIM_USER);
    conf->setValue("bAlwaysActive", ui->ckbAlwaysActive->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbIntro_change()
{
	if(init) return;
    conf->startSection("General");
    if(ui->ckbIntro->isChecked())
    {
        conf->setValue("sIntroSequence", QString("BGS_LOGO.BIK"), SKYRIM_USER);
    }
    else
    {
        conf->setValue("sIntroSequence", QString(""), SKYRIM_USER);
    }
}
void MainWindow::ckbScreenshot_change()
{
	if(init) return;
    conf->startSection("Display");
    conf->setValue("bAllowScreenshot", ui->ckbScreenshot->isChecked(), SKYRIM_USER);
}
void MainWindow::ckbFileSelection_change()
{
	if(init) return;
    conf->startSection("Launcher");
    conf->setValue("bEnableFileSelection", ui->ckbFileSelection->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbSavePause_change()
{
	if(init) return;
    conf->startSection("MAIN");
    conf->setValue("bSaveOnPause", ui->ckbSavePause->isChecked() && ui->grpAutoSave->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbSaveWait_change()
{
	if(init) return;
    conf->startSection("MAIN");
    conf->setValue("bSaveOnWait", ui->ckbSaveWait->isChecked() && ui->grpAutoSave->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbSaveRest_change()
{
	if(init) return;
    conf->startSection("MAIN");
    conf->setValue("bSaveOnRest", ui->ckbSaveRest->isChecked() && ui->grpAutoSave->isChecked(), SKYRIM_MAIN);
}
void MainWindow::ckbSaveTravel_change()
{
	if(init) return;
    conf->startSection("MAIN");
    conf->setValue("bSaveOnTravel", ui->ckbSaveTravel->isChecked() && ui->grpAutoSave->isChecked(), SKYRIM_MAIN);
}
void MainWindow::spnAutoSaveInt_change()
{
	if(init) return;
    conf->startSection("SaveGame");
    conf->setValue("fAutosaveEveryXMins", ui->spnAutoSaveInt->value(), SKYRIM_MAIN);
}
void MainWindow::grpAutoSave_change()
{
    if(init) return;
    conf->startSection("SaveGame");
    conf->setValue("bDisableAutoSave", !ui->grpAutoSave->isChecked(), SKYRIM_MAIN);
}

void MainWindow::ckbBckgrndLoadLip()
{
    if(init) return;
    conf->startSection("BackgroundLoad");
    conf->setValue("bBackgroundLoadLipFiles", ui->ckbBckgrndLoadLip->isChecked(), SKYRIM_MAIN);
}

void MainWindow::ckbBckgrndLoadFaceGen()
{
    if(init) return;
    conf->startSection("BackgroundLoad");
    conf->setValue("bLoadBackgroundFaceGen", ui->ckbBckgrndLoadFaceGen->isChecked(), SKYRIM_MAIN);
}
