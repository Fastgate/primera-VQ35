#ifndef ACM_H
#define ACM_H

class Acm {
  public:
    Acm() {}
    boolean isOtgActive() {
      return this->otg->getState() == HIGH;
    }
    boolean isHubActive() {
      return this->hub->getState() == HIGH;
    }
    void setOtg(boolean newState) {
      this->otg->toggle(newState);
    }
    void setHub(boolean newState) {
      this->hub->toggle(newState);
    }
  private:
    DigitalOutput *otg = new DigitalOutput(55, HIGH);
    DigitalOutput *hub = new DigitalOutput(54, HIGH);
};

#endif
