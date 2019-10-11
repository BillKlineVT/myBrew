#The recipe gives simple implementation of a Discrete Proportional-Integral-Derivative (PID) controller. PID controller gives output value for error between desired reference input and measurement feedback to minimize error value.
#More information: http://en.wikipedia.org/wiki/PID_controller
#
#cnr437@gmail.com
#
#######	Example	#########
#
#p=PID(3.0,0.4,1.2)
#p.setPoint(5.0)
#while True:
#     pid = p.update(measurement_value)
#
#
#updated by Bill Klinefelter
#bill.klinefelter@gmail.com
import time

class PID:
	"""
	Discrete PID control
	"""

	def __init__(self, P=2.0, I=0.0, D=1.0, Derivator=0, Integrator=0, Integrator_max=100, Integrator_min=-100, min_output=0, max_output=100):

		self.Kp=P
		self.Ki=I
		self.Kd=D
		self.Derivator=Derivator
		self.Integrator=Integrator
		self.Integrator_max=Integrator_max
		self.Integrator_min=Integrator_min
		self.min_output=min_output
                self.max_output=max_output

		self.set_point=0.0
		self.error=0.0
                self.last_cur_val=0.0
                self.lastsampletime=0

	def update(self,current_value):
		"""
		Calculate PID output value for given reference input and feedback
		"""

		self.error = self.set_point - current_value

		self.P_value = self.Kp * self.error
		
                #time_now = int(time.time())
                #deltatime = time_now - self.lastsampletime
                self.Derivator = ( current_value - self.last_cur_val )
                self.D_value = self.Kd * self.Derivator
                #self.lastsampletime = time_now
                self.last_cur_val = current_value

		self.Integrator = self.Integrator + self.error

		if self.Integrator > self.Integrator_max:
			self.Integrator = self.Integrator_max
		elif self.Integrator < self.Integrator_min:
			self.Integrator = self.Integrator_min

		self.I_value = self.Integrator * self.Ki

		PID = self.P_value + self.I_value - self.D_value

                # enforce max/min output vals
		if PID > self.max_output:
                    PID = self.max_output
                elif PID < self.min_output:
                    PID = self.min_output

                # correct i value based on max/min vals
                if self.I_value > self.max_output:
                    self.I_value = self.max_output
                elif self.I_value < self.min_output:
                    self.I_value = self.min_output


		return PID

	def setPoint(self,set_point):
		"""
		Initilize the setpoint of PID
		"""
		self.set_point = set_point
		self.Integrator=0
		self.Derivator=0

	def setIntegrator(self, Integrator):
		self.Integrator = Integrator

	def setDerivator(self, Derivator):
		self.Derivator = Derivator

	def setKp(self,P):
		self.Kp=P

	def setKi(self,I):
		self.Ki=I

	def setKd(self,D):
		self.Kd=D
        
        def setMaxOutput(self,maxoutput):
                self.max_output=maxoutput

        def setMinOutput(self,minoutput):
                self.min_output=minoutput

	def getPoint(self):
		return self.set_point
        
        def getP_val(self):
                return self.Kp

        def getI_val(self):
                return self.Ki 

        def getD_val(self):
                return self.Kd 

	def getError(self):
		return self.error

	def getIntegrator(self):
		return self.Integrator

	def getDerivator(self):
		return self.Derivator

