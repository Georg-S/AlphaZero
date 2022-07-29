#include "MultiThreadingNeuralNetManager.h"

MultiThreadingNeuralNetManager::MultiThreadingNeuralNetManager(int threadCount, int activeThreads, NeuralNetwork* net)
	: m_threadCount(threadCount)
	, m_activeThreads(activeThreads)
	, m_net(net)
	, m_elementsAdded(0)
{
}

int MultiThreadingNeuralNetManager::addInputThreadSafe(torch::Tensor input)
{
	std::unique_lock<std::mutex> lock(m_threadingMutex);
	const int resultIndex = m_elementsAdded;

	if (m_inputBuffer.numel() == 0)
		m_inputBuffer = input;
	else
		m_inputBuffer = torch::cat({ m_inputBuffer, input }, 0);
	m_elementsAdded++;

	return resultIndex;
}

std::tuple<torch::Tensor, torch::Tensor> MultiThreadingNeuralNetManager::getOutput(int index)
{
	torch::Tensor valueTens = std::get<0>(m_output)[index];
	torch::Tensor probTens = std::get<1>(m_output)[index];

	return std::make_tuple(valueTens, probTens);
}

void MultiThreadingNeuralNetManager::safeDecrementActiveThreads()
{
	std::unique_lock<std::mutex> lck(m_threadingMutex);

	m_activeThreads--;
	if ((m_activeThreads != 0) && (m_waitingThreads >= m_activeThreads))
		calculateAndWakeup();
}

void MultiThreadingNeuralNetManager::handleWaitingAndWakeup()
{
	std::unique_lock<std::mutex> lck(m_threadingMutex);
	if ((m_waitingThreads + 1) != m_activeThreads)
	{
		m_waitingThreads++;
		m_cond.wait(lck); // TODO somehow handle spurious wakeup
	}
	else
	{
		calculateAndWakeup();
	}
}

void MultiThreadingNeuralNetManager::calculateAndWakeup()
{
	calculateOutput();
	clearInput();
	wakeUpAllThreads();
}

int MultiThreadingNeuralNetManager::getThreadCount() const
{
	return m_threadCount;
}

void MultiThreadingNeuralNetManager::calculateOutput()
{
	m_output = m_net->calculate(m_inputBuffer);
}

void MultiThreadingNeuralNetManager::clearInput()
{
	m_inputBuffer = {};
	m_elementsAdded = 0;
}

void MultiThreadingNeuralNetManager::wakeUpAllThreads()
{
	m_waitingThreads = 0;
	m_cond.notify_all();
}