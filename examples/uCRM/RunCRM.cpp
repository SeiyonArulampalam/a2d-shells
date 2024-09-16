#include "TACSMeshLoader.h"
#include "TACSAssembler.h"

// this example is based off of examples/crm/crm.cpp in TACS

int main() {
    // build the TACS mesh loader and scan the uCRM BDF file
    printf("Scanning BDF file\n");
    TACSMeshLoader *mesh = new TACSMeshLoader();
    mesh->incref();
    mesh->scanBDFFile("CRM_box_2nd.bdf");

    // TODO : need to make an explicit shell element here which will be copied
    // for all conn / elements in the mesh (make this before creating TACS)

    // make the TACS assembler
    printf("Creating TACS assembler\n");
    TACSAssembler *assembler = mesh->createTACS(6);

    // TODO : add element, conn info to it


    printf("Done with RunCRM!\n");
}