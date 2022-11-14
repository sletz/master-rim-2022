#include "plugin.hpp"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/* Generateur de créneau */

float* table_square;
int table_size_square;
int phase_square;

/* Création et remplissage de la table avec un période du signal */
void init_square(int sample_rate, int freq)
{
    table_size_square = (int)((float)sample_rate / (float)freq);
    
    printf("table_size_square: %d\n", table_size_square);
    
    /* Alloue dynamiquement une table. */
    table_square = (float*)malloc(table_size_square * sizeof(float));
    
    /* Remplit la table */
    int i;
        // Génération de la 1° moitié
    for (i = 0; i < table_size_square/2; i = i + 1) {
        table_square[i] = -1.f;
    }
        // Génération de la 2° moitié
    for (i = table_size_square/2; i < table_size_square; i = i + 1) {
        table_square[i] = 1.f;
    }
    
    /* Initialise la phase */
    phase_square = 0;
}

void destroy_square()
{
    /* Desalloue le tableau. */
    free(table_square);
}

/* Copie 'nframes' echantillons de la table dans le buffer output et gestion de la phase */
void process_square(float* output, int nframes)
{
    int i;
    for (i = 0; i < nframes; i++) {
        output[i] = table_square[phase_square];
        phase_square = phase_square + 1;
        if (phase_square == table_size_square) {
            phase_square = 0;
        }
    }
}

float process_one_sample_square()
{
    float res = table_square[phase_square];
    phase_square = phase_square + 1;
    if (phase_square == table_size_square) {
        phase_square = 0;
    }
    return res;
}

void display_square()
{
    int i;
    for (i = 0; i < table_size_square; i++) {
        printf("Sample %f\n", table_square[i]);
    }
}

/* Generateur sinus */

float* table_sinus;
int table_size_sinus;
int phase_sinus;

/* Création et remplissage de la table avec un période du signal */
void init_sinus(int sample_rate, int freq)
{
    table_size_sinus = (int)((float)sample_rate / (float)freq);
    
    /* Alloue dynamiquement une table. */
    table_sinus = (float*)malloc(table_size_sinus * sizeof(float));
    
    /* Remplit la table */
    int i;
    for (i = 0; i < table_size_sinus; i++) {
        table_sinus[i] = (float)sin(2.f * M_PI * i/(float)table_size_sinus);
    }
    
    /* Initialise la phase */
    phase_sinus = 0;
}

void destroy_sinus()
{
    /* Desalloue le tableau. */
    free(table_sinus);
}

/* Copie 'nframes' echantillons de la table dans le buffer output et gestion de la phase */
void process_sinus(float* output, int nframes)
{
    int i;
    for (i = 0; i < nframes; i++) {
        output[i] = table_sinus[phase_sinus];
        phase_sinus = phase_sinus + 1;
        if (phase_sinus == table_size_sinus) {
            phase_sinus = 0;
        }
    }
}

/* Retourne 1 echantillon et gestion de la phase */
float process_one_sample_sinus()
{
    float res = table_sinus[phase_sinus];
    phase_sinus = phase_sinus + 1;
    if (phase_sinus == table_size_sinus) {
        phase_sinus = 0;
    }
    return res;
}

void display_sinus()
{
    int i;
    for (i = 0; i < table_size_sinus; i++) {
        printf("Sample %f\n", table_sinus[i]);
    }
}

struct MyModule : Module {
    enum ParamId {
        PARAM1_PARAM,
        PARAM2_PARAM,
        PARAM3_PARAM,
        PARAM4_PARAM,
        PARAMS_LEN
    };
    enum InputId {
        INPUT1_INPUT,
        INPUT2_INPUT,
        INPUTS_LEN
    };
    enum OutputId {
        OUTPUT1_OUTPUT,
        OUTPUT2_OUTPUT,
        OUTPUTS_LEN
    };
    enum LightId {
        LIGHTS_LEN
    };

    MyModule() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configParam(PARAM1_PARAM, 0.f, 1.f, 0.f, "");
        configParam(PARAM2_PARAM, 0.f, 1.f, 0.f, "");
        configParam(PARAM3_PARAM, 0.f, 1.f, 0.f, "");
        configParam(PARAM4_PARAM, 0.f, 1.f, 0.f, "");
        configInput(INPUT1_INPUT, "");
        configInput(INPUT2_INPUT, "");
        configOutput(OUTPUT1_OUTPUT, "");
        configOutput(OUTPUT2_OUTPUT, "");
        
        // Initialisation pour output1
        init_square(44100, 200);
        // Initialisation pour output2
        init_sinus(44100, 500);
    }
    
    virtual ~MyModule() {
        // Destruction
        destroy_square();
        destroy_sinus();
    }

    void process(const ProcessArgs& args) override {
        // Récupérer le valeur du 1° control
        float vol1 = params[PARAM1_PARAM].getValue();
        // Récupérer le valeur du 2° control
        float vol2 = params[PARAM2_PARAM].getValue();
        
        // Square sur output1 avec controle de volume
        outputs[OUTPUT1_OUTPUT].setVoltage(process_one_sample_square() * vol1 * 5.f);
        // Sinus sur output2 avec controle de volume
        outputs[OUTPUT2_OUTPUT].setVoltage(process_one_sample_sinus() * vol2 * 5.f);
    }
};


struct MyModuleWidget : ModuleWidget {
    MyModuleWidget(MyModule* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/MyModule.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.002, 21.161)), module, MyModule::PARAM1_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(22.705, 21.161)), module, MyModule::PARAM2_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.877, 42.705)), module, MyModule::PARAM3_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(22.92, 42.857)), module, MyModule::PARAM4_PARAM));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.569, 75.336)), module, MyModule::INPUT1_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.894, 75.336)), module, MyModule::INPUT2_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.323, 100.332)), module, MyModule::OUTPUT1_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.92, 100.332)), module, MyModule::OUTPUT2_OUTPUT));
    }
};


Model* modelMyModule = createModel<MyModule, MyModuleWidget>("MyModule");
