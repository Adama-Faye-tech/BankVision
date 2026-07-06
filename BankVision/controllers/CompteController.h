#pragma once

class CompteController {
public:
    CompteController();
    bool depot(int compteId, double montant);
    bool retrait(int compteId, double montant);
    bool transfert(int fromId, int toId, double montant);
};
