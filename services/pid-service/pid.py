"""
Python 3 port of python/PID_PWM_Controller/PID.py
Original by cnr437@gmail.com, updated by Bill Klinefelter.
Logic unchanged; Python 2 syntax cleaned up.
"""


class PID:
    """Discrete PID controller."""

    def __init__(
        self,
        P: float = 2.0,
        I: float = 0.0,
        D: float = 1.0,
        Derivator: float = 0,
        Integrator: float = 0,
        Integrator_max: float = 100,
        Integrator_min: float = -100,
        min_output: float = 0,
        max_output: float = 100,
    ):
        self.Kp = P
        self.Ki = I
        self.Kd = D
        self.Derivator = Derivator
        self.Integrator = Integrator
        self.Integrator_max = Integrator_max
        self.Integrator_min = Integrator_min
        self.min_output = min_output
        self.max_output = max_output

        self.set_point = 0.0
        self.error = 0.0
        self.last_cur_val = 0.0
        self.lastsampletime = 0

        self.P_value = 0.0
        self.I_value = 0.0
        self.D_value = 0.0

    def update(self, current_value: float) -> float:
        """Calculate PID output for given measurement."""
        self.error = self.set_point - current_value

        self.P_value = self.Kp * self.error

        self.Derivator = current_value - self.last_cur_val
        self.D_value = self.Kd * self.Derivator
        self.last_cur_val = current_value

        self.Integrator += self.error
        self.Integrator = max(self.Integrator_min, min(self.Integrator_max, self.Integrator))
        self.I_value = self.Integrator * self.Ki

        output = self.P_value + self.I_value - self.D_value

        # Clamp to output limits
        output = max(self.min_output, min(self.max_output, output))

        # Clamp I term as well
        self.I_value = max(self.min_output, min(self.max_output, self.I_value))

        return output

    def setPoint(self, set_point: float):
        self.set_point = set_point
        self.Integrator = 0
        self.Derivator = 0

    def setIntegrator(self, value: float):
        self.Integrator = value

    def setDerivator(self, value: float):
        self.Derivator = value

    def setKp(self, P: float):
        self.Kp = P

    def setKi(self, I: float):
        self.Ki = I

    def setKd(self, D: float):
        self.Kd = D

    def setMaxOutput(self, value: float):
        self.max_output = value

    def setMinOutput(self, value: float):
        self.min_output = value

    def getPoint(self) -> float:
        return self.set_point

    def getP_val(self) -> float:
        return self.Kp

    def getI_val(self) -> float:
        return self.Ki

    def getD_val(self) -> float:
        return self.Kd

    def getError(self) -> float:
        return self.error

    def getIntegrator(self) -> float:
        return self.Integrator

    def getDerivator(self) -> float:
        return self.Derivator
